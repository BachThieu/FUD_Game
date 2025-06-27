// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldPickup.h"
#include "FUD_Project/Character/MetaHumanCharacter.h"
#include "FUD_Project/MetaHumanComponents/BuffComponent.h"



void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	AMetaHumanCharacter* FUDCharacter = Cast<AMetaHumanCharacter>(OtherActor);
	if (FUDCharacter)
	{
		UBuffComponent* Buff = FUDCharacter->GetBuff();
		if (Buff)
		{
			Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
		}
	}
	Destroy();
}