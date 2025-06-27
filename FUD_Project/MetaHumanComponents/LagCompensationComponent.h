// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location = FVector(0.f, 0.f, 0.f);

	UPROPERTY()
	FRotator Rotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY()
	FVector BoxExtent = FVector(0.f, 0.f, 0.f);

};
USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time = 0.f;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfor;

	UPROPERTY()
	AMetaHumanCharacter* Character = nullptr;


};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed = false ;

	UPROPERTY()
	bool bHeadShot = false;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<AMetaHumanCharacter*, uint32> HeadShots;

	UPROPERTY()
	TMap<AMetaHumanCharacter*, uint32> BodyShots;


};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FUD_PROJECT_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULagCompensationComponent();
	friend class AMetaHumanCharacter;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);

	/*
	* Hitscan
	*/
	FServerSideRewindResult ServerSideRewind(class AMetaHumanCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);

	/*
	* projectile
	*/
	FServerSideRewindResult ProjectileServerSideRewind(
		AMetaHumanCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);


	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<AMetaHumanCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		AMetaHumanCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime
		);


	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		AMetaHumanCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime

	);

	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(
		const TArray<AMetaHumanCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	
	void CacheBoxPositions(AMetaHumanCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AMetaHumanCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AMetaHumanCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AMetaHumanCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(AMetaHumanCharacter* HitCharacter, float HitTime);
	/*
	* Hit Scan
	*/
	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package,
		AMetaHumanCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation);

	/*
	*  Projectile 
	*/
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		AMetaHumanCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);



	/*
	* Shot gun
	*/
	
	
	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage>& FramePackages,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations
	);
	 
private:
	UPROPERTY()
	AMetaHumanCharacter* Character;

	UPROPERTY()
	class AFUDPlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;

	

		
};
