// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FUDGameMode.generated.h"

namespace MatchState
{
	extern FUD_PROJECT_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer.
}

/**
 * 
 */
UCLASS()
class FUD_PROJECT_API AFUDGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AFUDGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AMetaHumanCharacter* ElimmedCharacter, class AFUDPlayerController* VictimController, class AFUDPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(class AFUDPlayerState* PlayerLeaving);
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);


	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStaringTime = 0.f;

	bool bTeamsMatch = false;
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
private:
	float CountdownTime = 0.f;
public:

	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }

};
