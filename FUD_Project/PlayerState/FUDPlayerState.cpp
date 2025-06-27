// Fill out your copyright notice in the Description page of Project Settings.


#include "FUDPlayerState.h"
#include "FUD_Project/Character/MetaHumanCharacter.h"
#include "FUD_Project/PlayerController/FUDPlayerController.h"
#include "Net/UnrealNetwork.h"


void AFUDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFUDPlayerState, Defeats);
	DOREPLIFETIME(AFUDPlayerState, Team);

}	

void AFUDPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<AMetaHumanCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AFUDPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
		
	}
}

void AFUDPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
		Character = Character == nullptr ? Cast<AMetaHumanCharacter>(GetPawn()) : Character;
		if (Character)
		{
			Controller = Controller == nullptr ? Cast<AFUDPlayerController>(Character->Controller) : Controller;
			if (Controller)
			{
				Controller->SetHUDDefeats(Defeats);
			}
		}
	
		
}

void AFUDPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}



void AFUDPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
}


void AFUDPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	Character = Character == nullptr ? Cast<AMetaHumanCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AFUDPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AFUDPlayerState::OnRep_Defeats()
{
	if (Controller)
	{
		Controller->SetHUDDefeats(Defeats);
	}
}


void AFUDPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;
	AMetaHumanCharacter* FCharacter = Cast<AMetaHumanCharacter>(GetPawn());
	if (FCharacter)
	{
		FCharacter->SetTeamColor(Team);
	}

}
void AFUDPlayerState::OnRep_Team()
{
	AMetaHumanCharacter* FCharacter = Cast<AMetaHumanCharacter>(GetPawn());
	if (FCharacter)
	{
		FCharacter->SetTeamColor(Team);
	}
}
