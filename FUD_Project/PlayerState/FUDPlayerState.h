// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FUD_Project\MetahumanTypes/Team.h"

#include "FUDPlayerState.generated.h"


/**
 * 
 */
UCLASS()
class FUD_PROJECT_API AFUDPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	* Replication notifies
	*/
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();

	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
private:  
	UPROPERTY()
	class AMetaHumanCharacter* Character;
	

	UPROPERTY()
	class AFUDPlayerController* Controller;


	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
	void OnRep_Team();
protected:
   virtual void BeginPlay() override;

};
