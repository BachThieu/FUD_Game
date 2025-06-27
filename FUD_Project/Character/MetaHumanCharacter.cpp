// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaHumanCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "FUD_Project/Weapon/Weapon.h"
#include "FUD_Project/MetaHumanComponents/CombatComponent.h"
#include "FUD_Project/MetaHumanComponents/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MetaHumanAnimation.h"
#include "FUD_Project/FUD_Project.h"
#include "FUD_Project/PlayerController/FUDPlayerController.h"
#include "FUD_Project/GameMode/FUDGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "FUD_Project/PlayerState/FUDPlayerState.h"
#include "FUD_Project/Weapon/WeaponTypes.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "FUD_Project/MetaHumanComponents/LagCompensationComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FUD_Project/GameState/FUDGameState.h"

// Sets default values
AMetaHumanCharacter::AMetaHumanCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));


	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); 
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);



	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolveTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Feet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	Feet->SetupAttachment(GetMesh());
	Feet->SetMasterPoseComponent(GetMesh(), true);
	Feet->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Legs= CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	Legs->SetupAttachment(GetMesh());
	Legs->SetMasterPoseComponent(GetMesh(), true);
	Legs->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Torso"));
	Torso->SetupAttachment(GetMesh());
	Torso->SetMasterPoseComponent(GetMesh(), true);
	Torso->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Face = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	Face->SetupAttachment(GetMesh());
	Face->SetMasterPoseComponent(GetMesh(), true);
	Face->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Eyelashes = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyelashes"));
	Eyelashes->SetupAttachment(Face);
	Eyelashes->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Fuzz = CreateDefaultSubobject<UGroomComponent>(TEXT("Fuzz"));
	Fuzz->SetupAttachment(Face);
	Fuzz->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(Face);
	Eyebrows->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(Face);
	Hair->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Mustache = CreateDefaultSubobject<UGroomComponent>(TEXT("Mustache"));
	Mustache->SetupAttachment(Face);
	Mustache->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Beard = CreateDefaultSubobject<UGroomComponent>(TEXT("Beard"));
	Beard->SetupAttachment(Face);
	Beard->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	/*
	* Hit boxes for server-side rewind
	*/

	
	head_n = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head_n->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head_n);

	

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_01 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_01"));
	spine_01->SetupAttachment(GetMesh(), FName("spine_01"));
	HitCollisionBoxes.Add(FName("spine_01"), spine_01);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	spine_04 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_04"));
	spine_04->SetupAttachment(GetMesh(), FName("spine_04"));
	HitCollisionBoxes.Add(FName("spine_04"), spine_04);

	spine_05 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_05"));
	spine_05->SetupAttachment(GetMesh(), FName("spine_05"));
	HitCollisionBoxes.Add(FName("spine_05"), spine_05);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);


	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);


	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	thig_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thig_l"));
	thig_l->SetupAttachment(GetMesh(), FName("thig_l"));
	HitCollisionBoxes.Add(FName("thig_l"), thig_l);

	thig_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thig_r"));
	thig_r->SetupAttachment(GetMesh(), FName("thig_r"));
	HitCollisionBoxes.Add(FName("thig_r"), thig_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);
	
	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		}
	}






}

void AMetaHumanCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMetaHumanCharacter, OverlappingWeapon,COND_OwnerOnly);
	DOREPLIFETIME(AMetaHumanCharacter, Health);
	DOREPLIFETIME(AMetaHumanCharacter, Shield);
	DOREPLIFETIME(AMetaHumanCharacter, bDisableGamePlay);

}

void AMetaHumanCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void AMetaHumanCharacter::Elim(bool bPlayerLeftGame)
{
	DropOrDestroyWeapons();

	MulticastElim(bPlayerLeftGame);
	
}

void AMetaHumanCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;

	if (MetahumanPlayerController)
	{
		MetahumanPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();
	
	// Start dissolve effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		Face->SetMaterial(0, DynamicDissolveMaterialInstance);
		Face->SetMaterial(1, DynamicDissolveMaterialInstance);
		Face->SetMaterial(3, DynamicDissolveMaterialInstance);
		Face->SetMaterial(4, DynamicDissolveMaterialInstance);
		Face->SetMaterial(9, DynamicDissolveMaterialInstance);
		Face->SetMaterial(10, DynamicDissolveMaterialInstance);
		Legs->SetMaterial(0, DynamicDissolveMaterialInstance);
		Torso->SetMaterial(0, DynamicDissolveMaterialInstance);
		Feet->SetMaterial(0, DynamicDissolveMaterialInstance);
		Hair->SetVisibility(false);

		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), -0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 400.f);
	}
	StartDissolve();

	// Disable character movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGamePlay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Face->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Legs->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Feet->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Torso->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Hair->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	// Spawn elim bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent =  UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation()

		);
	}
	if(ElimBotSound)
	{ 
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}
	bool bHideSniperScope = IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedWeapon && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope) 
	{
		ShowSniperScopeWidget(false);
	}
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&AMetaHumanCharacter::ElimTimerFinished,
		ElimDelay
	);
}



void AMetaHumanCharacter::ServerLeaveGame_Implementation()
{
	FUDGameMode = FUDGameMode == nullptr ? GetWorld()->GetAuthGameMode<AFUDGameMode>() : FUDGameMode;
	FUDPlayerState = FUDPlayerState == nullptr?  GetPlayerState<AFUDPlayerState>() : FUDPlayerState;


	if (FUDGameMode && FUDPlayerState)
	{
		FUDGameMode->PlayerLeftGame(FUDPlayerState);
	}
}

void AMetaHumanCharacter::ElimTimerFinished()
{
	FUDGameMode = FUDGameMode == nullptr ? GetWorld()->GetAuthGameMode<AFUDGameMode>() : FUDGameMode;
	if (FUDGameMode && !bLeftGame)
	{
		FUDGameMode->RequestRespawn(this, Controller);
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
}
void AMetaHumanCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void AMetaHumanCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);

		}
	}
}

void AMetaHumanCharacter::Destroyed()
{
	Super::Destroyed();
	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}
	FUDGameMode = FUDGameMode == nullptr ? GetWorld()->GetAuthGameMode<AFUDGameMode>() : FUDGameMode;
	bool bMatchNotInProgress = FUDGameMode && FUDGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}

}
void AMetaHumanCharacter::MulticastGainedTheLead_Implementation()
{
	if (CrownSystem == nullptr) return;
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();

	}

}
void AMetaHumanCharacter::MulticastLostTheLead_Implementation()
{
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();

	}
}
void AMetaHumanCharacter::SetTeamColor(ETeam Team)
{
	if (Torso == nullptr && OriginalMaterial == nullptr) return;
	switch (Team)
	{ 
	case ETeam::ET_NoTeam:
		Torso->SetMaterial(0, OriginalMaterial);
		break;
	case ETeam::ET_BlueTeam:
		Torso->SetMaterial(0, BlueMaterial);
		break;
	case ETeam::ET_RedTeam:
		Torso->SetMaterial(0, RedMaterial);
		break;
	}

}
// Called when the game starts or when spawned
void AMetaHumanCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultWeapon();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AMetaHumanCharacter::ReceiveDamage);
	} 

	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
	
	

	
}

// Called every frame
void AMetaHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateInPlace(DeltaTime);

	HideCameraIfCharacterClose();
	PollInit();
	
}
void AMetaHumanCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGamePlay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();

	}

}

// Called to bind functionality to input
void AMetaHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMetaHumanCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMetaHumanCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMetaHumanCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMetaHumanCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMetaHumanCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMetaHumanCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMetaHumanCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Released, this, &AMetaHumanCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AMetaHumanCharacter::GrenadeButtonPressed);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMetaHumanCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMetaHumanCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMetaHumanCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMetaHumanCharacter::LookUp);
}
void AMetaHumanCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<AFUDPlayerController>(Controller);

		}
	}
}

void AMetaHumanCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
		
	}


}

void AMetaHumanCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;

		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("ReloadPistol");
			break;

		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("ReloadPistol");
			break;
		case EWeaponType::EWT_ShotGun:
			SectionName = FName("ShotGunReload");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AMetaHumanCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);


	}

}

void AMetaHumanCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void AMetaHumanCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void AMetaHumanCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);

	}
}

void AMetaHumanCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

void AMetaHumanCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	
	FUDGameMode = FUDGameMode == nullptr ? GetWorld()->GetAuthGameMode<AFUDGameMode>() : FUDGameMode;
	if (bElimmed || FUDGameMode == nullptr) return;
	Damage = FUDGameMode->CalculateDamage(InstigatorController, Controller, Damage);

	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f,Damage);
			Shield = 0.f;  
		}
	}



	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();
	if (Health == 0.f)
	{
		
		if (FUDGameMode)
		{
			MetahumanPlayerController = MetahumanPlayerController == nullptr ? Cast<AFUDPlayerController>(Controller) : MetahumanPlayerController;
			AFUDPlayerController* AttackerController = Cast<AFUDPlayerController>(InstigatorController);
			FUDGameMode->PlayerEliminated(this, MetahumanPlayerController, AttackerController);
		}
	}
	
}




void AMetaHumanCharacter::MoveForward(float Value)
{
	if (bDisableGamePlay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);

	}

}

void AMetaHumanCharacter::MoveRight(float Value)
{
	if (bDisableGamePlay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);

	}
}

void AMetaHumanCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMetaHumanCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMetaHumanCharacter::EquipButtonPressed()
{
	if (bDisableGamePlay) return;
	if (Combat  )
	{
		 if(Combat->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();
		bool bSwap = Combat->ShouldSwapWeapon() && !HasAuthority() && Combat->CombatState == ECombatState::ECS_Unoccupied && OverlappingWeapon == nullptr;
		if (bSwap)
		{
			PlaySwapMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishedSwapping = false;
		}
	}   

}

void AMetaHumanCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapon())
		{
			Combat->SwapWeapons();
		}
	}
}
void AMetaHumanCharacter::CrouchButtonPressed()
{
	if (bDisableGamePlay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
	
}

void AMetaHumanCharacter::ReloadButtonPressed()
{
	if (bDisableGamePlay) return;
	if (Combat)
	{
		Combat->Reload();
	}
}

void AMetaHumanCharacter::AimButtonPressed()
{
	if (bDisableGamePlay) return;
	if (Combat)
	{
		Combat->SetAiming(true);

	}
}

void AMetaHumanCharacter::AimButtonReleased()
{
	if (bDisableGamePlay) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}
float AMetaHumanCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();

}

void AMetaHumanCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)// standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	CalculateAO_Pitch();
}
void AMetaHumanCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90,0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);

	}
}
void AMetaHumanCharacter::SimProxiesTurn()
{
	if (Combat == nullptr|| Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw; 
	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
} 
void AMetaHumanCharacter::Jump()
{
	if (bDisableGamePlay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else

	{
		Super::Jump();
	}
}

void AMetaHumanCharacter::FireButtonPressed()
{
	if (bDisableGamePlay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
	
	
}

void AMetaHumanCharacter::FireButtonReleased()
{
	if (bDisableGamePlay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}

}

void AMetaHumanCharacter::TurnInPlace(float DeltaTime)
{
	
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterAO_Yaw = FMath::FInterpTo(InterAO_Yaw, 0.f, DeltaTime, 10.f);
		AO_Yaw = InterAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}

	}
}


void AMetaHumanCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		Feet->SetVisibility(false);
		Torso->SetVisibility(false);
		Face->SetVisibility(false);
		Legs->SetVisibility(false);
	
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		Feet->SetVisibility(true);
		Torso->SetVisibility(true);
		Face->SetVisibility(true);
		Legs->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}

	 
}

void AMetaHumanCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
	
}
void AMetaHumanCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();
	}
}
void AMetaHumanCharacter::UpdateHUDHealth()
{
	MetahumanPlayerController = MetahumanPlayerController == nullptr ? Cast<AFUDPlayerController>(Controller) : MetahumanPlayerController;
	if (MetahumanPlayerController)
	{
		MetahumanPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}
void  AMetaHumanCharacter::UpdateHUDShield()
{
	MetahumanPlayerController = MetahumanPlayerController == nullptr ? Cast<AFUDPlayerController>(Controller) : MetahumanPlayerController;
	if (MetahumanPlayerController)
	{
		MetahumanPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void AMetaHumanCharacter::UpdateHUDAmmo()
{
	MetahumanPlayerController = MetahumanPlayerController == nullptr ? Cast<AFUDPlayerController>(Controller) : MetahumanPlayerController;
	if (MetahumanPlayerController && Combat && Combat->EquippedWeapon)
	{
		
		MetahumanPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		MetahumanPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());

	}

}

void AMetaHumanCharacter::SpawnDefaultWeapon()
{
	FUDGameMode = FUDGameMode == nullptr ? GetWorld()->GetAuthGameMode<AFUDGameMode>() : FUDGameMode;
	UWorld* World = GetWorld();
	if (FUDGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);

		}
	}

}

void AMetaHumanCharacter::PollInit()
{
	if (FUDPlayerState == nullptr)
	{
		FUDPlayerState = GetPlayerState<AFUDPlayerState>();
		if (FUDPlayerState)
		{
			FUDPlayerState->AddToScore(0.f);
			FUDPlayerState->AddToDefeats(0);
			SetTeamColor(FUDPlayerState->GetTeam());
			AFUDGameState* FUDGameState = Cast<AFUDGameState>(UGameplayStatics::GetGameState(this));
			if (FUDGameState && FUDGameState->TopScoringPlayers.Contains(FUDPlayerState))
			{
				MulticastGainedTheLead();
			}
			UpdateHUDHealth();
		}
	}
}

void AMetaHumanCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue); 
	}
}

void AMetaHumanCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &AMetaHumanCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeLine)
	{
		DissolveTimeLine->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeLine->Play();
	}
}

void AMetaHumanCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}

	}
}

void AMetaHumanCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false); 
	}
}
bool AMetaHumanCharacter::IsWeaponEquipped()
{

	return (Combat && Combat->EquippedWeapon);

}

bool AMetaHumanCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* AMetaHumanCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector AMetaHumanCharacter::GetHitTarget() const
{
	//Hamnaykhongdungtoitrong rotation right hand
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

FVector AMetaHumanCharacter::GetEndHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitEndTarget;

}

ECombatState AMetaHumanCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}
bool AMetaHumanCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;

}

 






