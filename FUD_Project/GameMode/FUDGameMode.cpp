// Fill out your copyright notice in the Description page of Project Settings.


#include "FUDGameMode.h"
#include "FUD_Project/Character/MetaHumanCharacter.h"
#include "FUD_Project/PlayerController/FUDPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "FUD_Project/PlayerState/FUDPlayerState.h"
#include "FUD_Project/GameState/FUDGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}


AFUDGameMode::AFUDGameMode()
{
	bDelayedStart = true;
}

void AFUDGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStaringTime = GetWorld()->GetTimeSeconds();
}



void AFUDGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStaringTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStaringTime;
		if (CountdownTime<= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStaringTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}

}
void AFUDGameMode::OnMatchStateSet()
{   
	Super::OnMatchStateSet();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AFUDPlayerController* FUDPlayer = Cast<AFUDPlayerController>(*It);
		if (FUDPlayer)
		{ 
			FUDPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

float AFUDGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void AFUDGameMode::PlayerEliminated(AMetaHumanCharacter* ElimmedCharacter, AFUDPlayerController* VictimController, AFUDPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	AFUDPlayerState* AttackerPlayerState = AttackerController ? Cast <AFUDPlayerState>(AttackerController->PlayerState) : nullptr;
	AFUDPlayerState* VictimPlayerState = VictimController ? Cast <AFUDPlayerState>(VictimController->PlayerState) : nullptr;

	AFUDGameState* FUDGameState = GetGameState<AFUDGameState>();
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState  && FUDGameState)
	{
		TArray<AFUDPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : FUDGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);

		}


		AttackerPlayerState->AddToScore(1.0f);
		FUDGameState->UpdateTopScore(AttackerPlayerState);
		if (FUDGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			AMetaHumanCharacter* Leader = Cast<AMetaHumanCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}

		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!FUDGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				AMetaHumanCharacter* Loser = Cast<AMetaHumanCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}

			}
		}

	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AFUDPlayerController* FUDPlayer = Cast<AFUDPlayerController>(*It);
		if (FUDPlayer && AttackerPlayerState && VictimPlayerState)
		{
			FUDPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);

		}

	}

}

void AFUDGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0,PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
void AFUDGameMode::PlayerLeftGame(AFUDPlayerState* PlayerLeaving)
{
	 // TODO call elim., passing in true for bLeftGame
	if (PlayerLeaving == nullptr) return;
	AFUDGameState* FUDGameState = GetGameState<AFUDGameState>();
	if (FUDGameState && FUDGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		FUDGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	AMetaHumanCharacter* CharacterLeaving = Cast<AMetaHumanCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);

	}
}