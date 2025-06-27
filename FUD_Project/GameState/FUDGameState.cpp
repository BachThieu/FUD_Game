// Fill out your copyright notice in the Description page of Project Settings.


#include "FUDGameState.h"
#include "Net/UnrealNetwork.h"
#include "FUD_Project/PlayerState/FUDPlayerState.h"
#include "FUD_Project/PlayerController/FUDPlayerController.h"

void AFUDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AFUDGameState, TopScoringPlayers);
	DOREPLIFETIME(AFUDGameState, RedTeamScore);
	DOREPLIFETIME(AFUDGameState, BlueTeamScore);
}

void AFUDGameState::UpdateTopScore(AFUDPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}

}

void AFUDGameState::RedTeamScores()
{
	++RedTeamScore;

	AFUDPlayerController* BPlayer = Cast<AFUDPlayerController>(GetWorld()->GetFirstPlayerController()); 
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AFUDGameState::BlueTeamScores()
{
	++BlueTeamScore;
	AFUDPlayerController* BPlayer = Cast<AFUDPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}

}

void AFUDGameState::OnRep_RedTeamScore()
{
	AFUDPlayerController* BPlayer = Cast<AFUDPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AFUDGameState::OnRep_BlueTeamScore()
{
	AFUDPlayerController* BPlayer = Cast<AFUDPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}
}
