// Fill out your copyright notice in the Description page of Project Settings.


#include "FUDPlayerController.h"
#include "FUD_Project/HUD/FUDHUD.h"
#include "FUD_Project/HUD/CharacterOverlap.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FUD_Project/Character/MetaHumanCharacter.h"
#include "Net/UnrealNetwork.h"
#include "FUD_Project/GameMode/FUDGameMode.h"
#include "FUD_Project/PlayerState/FUDPlayerState.h"
#include "FUD_Project/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "FUD_Project/MetaHumanComponents/CombatComponent.h"
#include "FUD_Project/GameState/FUDGameState.h"
#include "Components/Image.h"
#include "FUD_Project/HUD/ReturntoMainMenu.h"
#include "FUD_Project/MetahumanTypes/Announcement.h"
 

void AFUDPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);

}

void AFUDPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Attacker && Victim && Self)
	{
		FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;
		if (FUDHUD)
		{
			if (Attacker == Self && Victim != Self)
			{
				FUDHUD->AddElimAnnouncement("You", Victim->GetPlayerName());
				return;

			}
			if (Victim == Self && Attacker != Self)
			{
				FUDHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "You");
				return;
			}
			if (Attacker == Victim && Attacker == Self)
			{
				FUDHUD->AddElimAnnouncement("You", "Yourself");
				return;
			}
			if (Attacker == Victim && Attacker != Self)
			{
				FUDHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "themselves;");
				return;

			}
			FUDHUD->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());

		}
	}
}
void AFUDPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FUDHUD = Cast<AFUDHUD>(GetHUD());
	ServerCheckMatchState();
	
}

void AFUDPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFUDPlayerController, MatchState);
	DOREPLIFETIME(AFUDPlayerController, bShowTeamScores);
}
void AFUDPlayerController::HideTeamScores()
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->RedTeamScore &&
		FUDHUD->CharacterOverlay->BlueTeamScore;

	if (bHUDValid)
	{
		FUDHUD->CharacterOverlay->RedTeamScore->SetText(FText());
		FUDHUD->CharacterOverlay->BlueTeamScore->SetText(FText());

	}
}
void AFUDPlayerController::InitTeamScores()
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->RedTeamScore &&
		FUDHUD->CharacterOverlay->BlueTeamScore;

	if (bHUDValid)
	{
		FString Zero("0");
		FUDHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(Zero));
		FUDHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(Zero));

	}
}
void AFUDPlayerController::SetHUDRedTeamScore(int32 RedScore)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->RedTeamScore;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), RedScore);
		FUDHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
		

	}
}
void AFUDPlayerController::SetHUDBlueTeamScore(int32 BlueScore)
{

	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->BlueTeamScore;

	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), BlueScore);
		FUDHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(ScoreText));


	}
}
void AFUDPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();

	CheckPing(DeltaTime);

}
void AFUDPlayerController::CheckPing(float DeltaTime)

{
	if (HasAuthority()) return;

	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			if (PlayerState->GetPing() * 4 > HighPingThreshold) //Ping is compressed; it is actually ping /4
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false); 
			}
		}
		HighPingRunningTime = 0.f;
	}
	bool bHighPingAnimationisPlaying = FUDHUD && FUDHUD->CharacterOverlay
		&& FUDHUD->CharacterOverlay->HighPingAnimation &&
		FUDHUD->CharacterOverlay->IsAnimationPlaying(FUDHUD->CharacterOverlay->HighPingAnimation);
	if (bHighPingAnimationisPlaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}

	}
}

void AFUDPlayerController::ShowReturnToMainMenu()
{
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturntoMainMenu>(this, ReturnToMainMenuWidget);

	 }
	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();

		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}
}

void AFUDPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitTeamScores();
	}
	else
	{
		HideTeamScores();
	}
}




 
void AFUDPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}
void AFUDPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AFUDPlayerController::HighPingWarning()
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->HighPingImage &&
		FUDHUD->CharacterOverlay->HighPingAnimation;

	if (bHUDValid)
	{
		
		FUDHUD->CharacterOverlay->HighPingImage->SetRenderOpacity(1.f);
		FUDHUD->CharacterOverlay->PlayAnimation(
			FUDHUD->CharacterOverlay->HighPingAnimation,
			0.f,
			5);
	}
}

void AFUDPlayerController::StopHighPingWarning()
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->HighPingImage &&
		FUDHUD->CharacterOverlay->HighPingAnimation;

	if (bHUDValid)
	{
		FUDHUD->CharacterOverlay->HighPingImage->SetRenderOpacity(1.f);
		if (FUDHUD->CharacterOverlay->IsAnimationPlaying(FUDHUD->CharacterOverlay->HighPingAnimation))
		{
			FUDHUD->CharacterOverlay->StopAnimation(FUDHUD->CharacterOverlay->HighPingAnimation);
		}	
	}
}



void AFUDPlayerController::ServerCheckMatchState_Implementation()
{
	AFUDGameMode* GameMode = Cast<AFUDGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmUpTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStaringTime = GameMode->LevelStaringTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmUpTime, MatchTime,CooldownTime, LevelStaringTime);

		if (FUDHUD && MatchState == MatchState::WaitingToStart)
		{
			FUDHUD->AddAnnouncement();
		}

	}
}
void AFUDPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmUpTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStaringTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (FUDHUD && MatchState == MatchState::WaitingToStart)
	{
		FUDHUD->AddAnnouncement();
	}
}
 

void AFUDPlayerController::OnPossess(APawn* InPawn)
{ 
	Super::OnPossess(InPawn);

	AMetaHumanCharacter* MetahumanCharacter = Cast<AMetaHumanCharacter>(InPawn);
	if (MetahumanCharacter)
	{
		SetHUDHealth(MetahumanCharacter->GetHealth(), MetahumanCharacter->GetMaxHealth());
	
	}
}



void AFUDPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("Quit", IE_Pressed, this, &AFUDPlayerController::ShowReturnToMainMenu);


}

void AFUDPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD && 
		FUDHUD->CharacterOverlay && 
		FUDHUD->CharacterOverlay->HealthBar && 
		FUDHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		FUDHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		FUDHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;

	}
}
void AFUDPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->ShieldBar &&
		FUDHUD->CharacterOverlay->ShieldText;

	if (bHUDValid)
	{
		const float ShieldPercent = Shield / MaxShield;
		FUDHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		FUDHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;

	}
}

void AFUDPlayerController::SetHUDScore(float Score)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		FUDHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeScore = true;
		HUDScore = Score;
	}

}

void AFUDPlayerController::SetHUDDefeats(int32 Defeats)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		FUDHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
	}
}

void AFUDPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		FUDHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;

	}
}

void AFUDPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;
	
	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		FUDHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void AFUDPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			FUDHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FUDHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
	
}
void AFUDPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->Announcement &&
		FUDHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			FUDHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FUDHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}

}
void AFUDPlayerController::SetHUDGrenades(int32 Grenades)
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;

	bool bHUDValid = FUDHUD &&
		FUDHUD->CharacterOverlay &&
		FUDHUD->CharacterOverlay->GrenadesText;
	if (bHUDValid)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		FUDHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
	}
}
void AFUDPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmUpTime - GetServerTime() + LevelStaringTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmUpTime + MatchTime - GetServerTime() + LevelStaringTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmUpTime + MatchTime - GetServerTime() + LevelStaringTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft); 
	if (HasAuthority())
	{
		FUDGameMode = FUDGameMode == nullptr ? Cast <AFUDGameMode>(UGameplayStatics::GetGameMode(this)) : FUDGameMode;
		if (FUDGameMode)
		{
			SecondsLeft = FMath::CeilToInt(FUDGameMode->GetCountdownTime() + LevelStaringTime);
		}
	}
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
		
	}
	CountdownInt = SecondsLeft;

}

void AFUDPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (FUDHUD && FUDHUD->CharacterOverlay)
		{
			CharacterOverlay = FUDHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
					
				 if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				 if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);		
				 if (bInitializeScore) SetHUDScore(HUDScore);
				 if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				 if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				 if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);

				AMetaHumanCharacter* FUDCharacter = Cast<AMetaHumanCharacter>(GetPawn());
				if (FUDCharacter && FUDCharacter->GetCombat())
				{
					if (bInitializeGrenades) SetHUDGrenades(FUDCharacter->GetCombat()->GetGrenades());
				}

			
			}
		}
	}

}


void AFUDPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}
void AFUDPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AFUDPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
	
}
void AFUDPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}
void AFUDPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState = State;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);

	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}
void AFUDPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}

}
void AFUDPlayerController::SetTimer()
{
	GetWorldTimerManager().SetTimer(
		TimerCast,
		this,
		&AFUDPlayerController::SetTimerFinish,
		0.25f
	);
}
void AFUDPlayerController::SetTimerFinish()
{
	SetHUDCarriedAmmo(5);
}
void AFUDPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	if(HasAuthority()) bShowTeamScores = bTeamsMatch;
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;
	if (FUDHUD)
	{
		if(FUDHUD->CharacterOverlay == nullptr) FUDHUD->AddCharacterOverlay();
		
		if (FUDHUD->Announcement)
		{
			FUDHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
		if (!HasAuthority()) return;
		if (bTeamsMatch)
		{
			InitTeamScores();
		}
		else
		{
			HideTeamScores();

		}
	}
}

void AFUDPlayerController::HandleCooldown()
{
	FUDHUD = FUDHUD == nullptr ? Cast<AFUDHUD>(GetHUD()) : FUDHUD;
	if (FUDHUD)
	{
		FUDHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = FUDHUD->Announcement && FUDHUD->Announcement->AnnouncementText && FUDHUD->Announcement->InforText;
		if (bHUDValid)
		{
			FUDHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText = Announcement::Newmatchstartsin;
			FUDHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			AFUDGameState* FUDGameState =  Cast<AFUDGameState>(UGameplayStatics::GetGameState(this));
			AFUDPlayerState* FUDPlayerState = GetPlayerState<AFUDPlayerState>();
			if (FUDGameState && FUDPlayerState)
			{
				TArray<AFUDPlayerState*> TopPlayers = FUDGameState->TopScoringPlayers;
				FString InforTextString = bShowTeamScores ? GetTeamsInfoText(FUDGameState) : GetInfoText(TopPlayers);
				FUDHUD->Announcement->InforText->SetText(FText::FromString(InforTextString));
			}
			
		}
	}
	AMetaHumanCharacter* MetahumanCharacter = Cast<AMetaHumanCharacter>(GetPawn());
	if (MetahumanCharacter && MetahumanCharacter->GetCombat())
	{
		MetahumanCharacter->bDisableGamePlay = true;
		MetahumanCharacter->GetCombat()->FireButtonPressed(false);
	}
}
FString AFUDPlayerController::GetInfoText(const TArray<class AFUDPlayerState*>& Players)
{
	AFUDPlayerState* FUDPlayerState = GetPlayerState<AFUDPlayerState>();
	if (FUDPlayerState == nullptr) return FString();

	FString InforTextString;
	if (Players.Num() == 0)
	{
		InforTextString = Announcement::Thereisnowinner;

	}
	else if (Players.Num() == 1 && Players[0] == FUDPlayerState)
	{
		InforTextString = Announcement::Youarethewinner;

	}
	else if (Players.Num() == 1)
	{
		InforTextString = FString::Printf(TEXT("Winner: \n%s"), *Players[0]->GetPlayerName());

	}
	else if (Players.Num() > 1)
	{
		InforTextString = Announcement::PlayersTiedForTheWin;
		InforTextString.Append(FString("\n"));
		for (auto TiedPlayer : Players)
		{
			InforTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
		}

	}
	return InforTextString;
}

FString AFUDPlayerController::GetTeamsInfoText(AFUDGameState* FUDGameState)
{
	if (FUDGameState == nullptr) return FString();

	FString InforTextString;

	const int32 RedTeamScore = FUDGameState->RedTeamScore;
	const int32 BlueTeamScore = FUDGameState->BlueTeamScore;

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InforTextString = Announcement::Thereisnowinner;
	}
	else if (RedTeamScore == BlueTeamScore)
	{
		InforTextString = FString::Printf(TEXT("&s\n"), *Announcement::TeamTiedForTheWin);
		InforTextString.Append(Announcement::RedTeam);
		InforTextString.Append(TEXT("\n"));
		InforTextString.Append(Announcement::BlueTeam);
		InforTextString.Append(TEXT("\n"));

	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InforTextString = Announcement::RedTeamWins;
		InforTextString.Append(TEXT("\n"));
		InforTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
		InforTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
	}
	else if (RedTeamScore < BlueTeamScore)
	{
		InforTextString = Announcement::BlueTeamWins;
		InforTextString.Append(TEXT("\n"));
		InforTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
		InforTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));

	}
	return InforTextString;
}
