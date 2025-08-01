// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FUDHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread; 
	FLinearColor CrosshairsColor;
};
/**
 * 
 */
UCLASS()
class FUD_PROJECT_API AFUDHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf< class UUserWidget> CharacterOverlayClass;
	void AddCharacterOverlay();
	
	UPROPERTY()
	class UCharacterOverlap* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf< class UUserWidget> AnnouncementClass;

	UPROPERTY()
	class UAnnouncement* Announcement;

	void AddAnnouncement();
	void AddElimAnnouncement(FString Attacker, FString Victim);


protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY()
	class APlayerController* OwningPlayer;

	FHUDPackage HUDPackage;
	
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 8.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime = 2.5f;

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;


public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package)
	{
		HUDPackage = Package;
	}

};
