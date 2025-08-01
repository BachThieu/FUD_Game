// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class FUD_PROJECT_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	// Destructor
	virtual ~APickup();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override; 
	virtual void BeginDestroy() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere)	
	float BaseTurnRate = 45.f;
private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;

	FTimerHandle BindOverlapTimer;

	float BindOverlapTime  = 0.25f;
	void BindOverlapTimerFinished();
public:	


};
