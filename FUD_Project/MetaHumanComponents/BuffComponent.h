// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FUD_PROJECT_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuffComponent();
	friend class AMetaHumanCharacter;
	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ReplenishTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float Velocity);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampup(float DeltaTime);

private:
	UPROPERTY()
	class AMetaHumanCharacter* Character;

	/*
	* Heal  buff
	*/
	 
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	/*
	* Shield buff
	*/
	bool bReplenishingShield;
	float ReplenishShieldRate = 0.f;
	float ShieldAmountToReplenish = 0.f;


	/*
	* Speed  buff
	*/

	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	/*
	* Jump buff
	*/

	
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);



public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
