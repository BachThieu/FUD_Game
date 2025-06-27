// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"
#include "FUD_Project/GameState/FUDGameState.h"
#include "FUD_Project/PlayerState/FUDPlayerState.h"
#include "FUD_Project/PlayerController/FUDPlayerController.h"
#include "Kismet/GameplayStatics.h"


ATeamGameMode::ATeamGameMode()
{
	bTeamsMatch = true;
}

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AFUDGameState* BGameState = Cast<AFUDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		AFUDPlayerState* BPState = NewPlayer->GetPlayerState<AFUDPlayerState>();
		if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
			{
				BGameState->RedTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_RedTeam);
				

			}
			else
			{
				BGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_BlueTeam);
				

			}
		}
		
	}
}
void ATeamGameMode::Logout(AController* Exiting)
{
	AFUDGameState* BGameState = Cast<AFUDGameState>(UGameplayStatics::GetGameState(this));
	AFUDPlayerState* BPState = Exiting->GetPlayerState<AFUDPlayerState>();
	if (BGameState && BPState)
	{
		if (BGameState->RedTeam.Contains(BPState))
		{
			BGameState->RedTeam.Remove(BPState);
		}
		if (BGameState->BlueTeam.Contains(BPState))
		{
			BGameState->BlueTeam.Remove(BPState);
		}
	}
}

void ATeamGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AFUDGameState* BGameState = Cast<AFUDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		for (auto PState : BGameState->PlayerArray)
		{
			AFUDPlayerState* BPState = Cast<AFUDPlayerState>(PState.Get());
			if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
				{
					BGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);

				}
			}
			else
			{
				BGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_BlueTeam);

			}
		}
	}

}
float ATeamGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) 
{
	AFUDPlayerState* AttackerPState = Attacker->GetPlayerState<AFUDPlayerState>();
	AFUDPlayerState* VictimPState = Victim->GetPlayerState<AFUDPlayerState>();
	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if (VictimPState == AttackerPState)
	{
		return BaseDamage;

	}
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return 0.f;
	}
	return BaseDamage;
}
void ATeamGameMode::PlayerEliminated(AMetaHumanCharacter* ElimmedCharacter, AFUDPlayerController* VictimController, AFUDPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
	AFUDGameState* BGameState = Cast<AFUDGameState>(UGameplayStatics::GetGameState(this));
	AFUDPlayerState* AttackerPlayerState = AttackerController ? Cast<AFUDPlayerState>(AttackerController->PlayerState) : nullptr;
	if (BGameState && AttackerPlayerState)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeamScores();

		}
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			BGameState->RedTeamScores();

		}
	}

}