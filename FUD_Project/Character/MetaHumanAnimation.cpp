// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaHumanAnimation.h"
#include "MetaHumanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "FUD_Project/Weapon/Weapon.h"
#include "FUD_Project/MetahumanTypes/CombatState.h"


void UMetaHumanAnimation::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MetaHumanCharacter = Cast<AMetaHumanCharacter>(TryGetPawnOwner());

}

void UMetaHumanAnimation::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MetaHumanCharacter == nullptr)
	{
		MetaHumanCharacter = Cast<AMetaHumanCharacter>(TryGetPawnOwner());
	}
	if (MetaHumanCharacter == nullptr) return;

	FVector Velocity = MetaHumanCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size(); 

	bIsInAir = MetaHumanCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = MetaHumanCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = MetaHumanCharacter->IsWeaponEquipped();
	EquippedWeapon = MetaHumanCharacter->GetEquippedWeapon();
	bIsCrouched = MetaHumanCharacter->bIsCrouched;
	bAiming = MetaHumanCharacter->IsAiming();
	TurningInPlace = MetaHumanCharacter->GetTurningInPlace();
	bRotateRootBone = MetaHumanCharacter->ShouldRotateRootBone();
	bElimmed = MetaHumanCharacter->IsElimmed();

	//Offset Yaw for Strafing
	FRotator AimRotation = MetaHumanCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MetaHumanCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;
	 
	
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = MetaHumanCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
	AO_Yaw = MetaHumanCharacter->GetAOYaw();
	AO_Pitch = MetaHumanCharacter->GetAOPitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && MetaHumanCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		MetaHumanCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));  

		if (MetaHumanCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			//Kho hieu dong duoi, rotation right hand fix bang cach rotate right hand socket cua weapon
			FTransform RightHandTransform = MetaHumanCharacter->GetMesh()->GetSocketTransform(FName("hand_r_transform"), ERelativeTransformSpace::RTS_World);
			RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - MetaHumanCharacter->GetEndHitTarget()));
			FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MetaHumanCharacter->GetHitTarget(), FColor::Orange);
		}	
	}
	bUseFABRIK = MetaHumanCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	bool bFABBIKOverride = MetaHumanCharacter->IsLocallyControlled()
		&& MetaHumanCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade &&
		MetaHumanCharacter->bFinishedSwapping;
	if (bFABBIKOverride)
	{
		bUseFABRIK = !MetaHumanCharacter->IsLocallyReloading();
	}
	bUseAimOffsets = MetaHumanCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !MetaHumanCharacter->GetDisableGameplay();
	bTransformRightHand = MetaHumanCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !MetaHumanCharacter->GetDisableGameplay();
}
