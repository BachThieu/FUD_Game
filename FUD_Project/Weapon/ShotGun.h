// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ShotGun.generated.h"

/**
 * 
 */
UCLASS()
class FUD_PROJECT_API AShotGun : public AHitScanWeapon
{
	GENERATED_BODY()
public:
	//virtual void Fire(const FVector& HitTarget) override;
	virtual void FireShotgun(const TArray<FVector_NetQuantize>& HitTargets);


	void ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets);


private:

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	uint32 NumberOfPellets = 10;
};
