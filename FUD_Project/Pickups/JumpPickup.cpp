// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPickup.h"
#include "FUD_Project/Character/MetaHumanCharacter.h"
#include "FUD_Project/MetaHumanComponents/BuffComponent.h"

void AJumpPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	AMetaHumanCharacter* FUDCharacter = Cast<AMetaHumanCharacter>(OtherActor);
	if (FUDCharacter)
	{
		UBuffComponent* Buff = FUDCharacter->GetBuff();
		if (Buff)
		{
			Buff->BuffJump(JumpZVelocityBuff, JumpBuffTime);
		}

	}
	Destroy();
}

