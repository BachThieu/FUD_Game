// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include "FUD_Project/Character/MetaHumanCharacter.h"
#include "FUD_Project/MetaHumanComponents/CombatComponent.h"

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	AMetaHumanCharacter* FUDCharacter = Cast<AMetaHumanCharacter>(OtherActor);
	if (FUDCharacter)
	{
		UCombatComponent* Combat = FUDCharacter->GetCombat();
		if (Combat)
		{
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Destroy();
}
 