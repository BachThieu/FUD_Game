// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "FUD_Project/Weapon/WeaponTypes.h"
#include "AmmoPickup.generated.h"


/**
 * 
 */
UCLASS()
class FUD_PROJECT_API AAmmoPickup : public APickup
{
	GENERATED_BODY()
protected:   

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;
	  
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

};
