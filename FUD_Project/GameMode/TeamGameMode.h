// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FUDGameMode.h"
#include "TeamGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FUD_PROJECT_API ATeamGameMode : public AFUDGameMode
{
	GENERATED_BODY()
protected:
	virtual void HandleMatchHasStarted() override;  
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
public:
	ATeamGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void PlayerEliminated(class AMetaHumanCharacter* ElimmedCharacter, class AFUDPlayerController* VictimController, class AFUDPlayerController* AttackerController);

};
