// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FUD_Project/MetahumanTypes/TurningInPlace.h"
#include "FUD_Project/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "FUD_Project/MetahumanTypes/CombatState.h"
#include "FUD_Project/MetahumanTypes/Team.h"
#include "MetaHumanCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);


UCLASS()
class FUD_PROJECT_API AMetaHumanCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMetaHumanCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	/*
	* Play montages
	*/
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapMontage();

	virtual void OnRep_ReplicatedMovement() override;

	void Elim(bool bPlayerLeftGame);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastElim(bool bPlayerLeftGame);

	virtual void Destroyed() override;

	UPROPERTY(Replicated)
		bool bDisableGamePlay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();

	void SpawnDefaultWeapon();

	UPROPERTY()
	TMap<FName, class  UBoxComponent*> HitCollisionBoxes;

	bool bFinishedSwapping = false;
	 
	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	void SetTeamColor(ETeam Team);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();

	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();
	void GrenadeButtonPressed();
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();



	//Bind function nhu overlap hay receivedamage se  k chay neu k co UFUNCTION()
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	
	//Poll for any relelvant classes and initialize our HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);

	/*
	*  Hit boxes used for server-side rewind
	*/
	
	

	UPROPERTY(EditAnywhere)
	class UBoxComponent* pelvis;


	UPROPERTY(EditAnywhere)
	UBoxComponent* head_n;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_01;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_04;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_05;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thig_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thig_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;





		


private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class USkeletalMeshComponent* Feet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class USkeletalMeshComponent* Legs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class USkeletalMeshComponent* Torso;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class USkeletalMeshComponent* Face;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class UGroomComponent* Eyelashes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class UGroomComponent* Fuzz;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class UGroomComponent* Eyebrows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class UGroomComponent* Hair;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class UGroomComponent* Mustache;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowprivateAccess = "true"))
		class UGroomComponent* Beard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowprivateAccess = "true"))
		class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUSing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;


	/*
	* FUD Component
	*/
	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;



	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/*
	* Animation montages
	*/

	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* ReloadMontage;


	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* SwapMontage;


	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
		float CameraThreshold = 200.f; 

	bool bRotateRootBone;

	UPROPERTY(EditAnywhere)
		float TurnThreshold = 0.5f;

	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/*
	* Play health
	*/

	UPROPERTY(EditAnywhere, Category = " Play Stats")
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
		float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	/*
	* Player Shield
	*/

	UPROPERTY(EditAnywhere, Category ="Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY()
	class AFUDPlayerController* MetahumanPlayerController;

	bool bElimmed = false;

	

	



	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
		float ElimDelay = 3.f;

	void ElimTimerFinished();

	bool bLeftGame = false;

	/*
	* Dissolve effect
	*/

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeLine;

	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
		UCurveFloat* DissolveCurve;


	UFUNCTION()
		void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	// Dynamic Instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = "Elim")
		UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	//Material instance set on the blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = "Elim")
		UMaterialInstance* DissolveMaterialInstance;


	/*
	* Team colors
	*/

	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* RedMaterial;

	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* BlueMaterial;

	UPROPERTY(EditAnywhere, Category = "Elim")
	UMaterialInstance* OriginalMaterial;

	/*
	* Elim effects
	*/
	UPROPERTY(EditAnywhere)
		UParticleSystem* ElimBotEffect;

	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
		class USoundCue* ElimBotSound;

	UPROPERTY()
		class AFUDPlayerState* FUDPlayerState;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;

	UPROPERTY()
	class UNiagaraComponent* CrownComponent;

	/*
	* Grenade
	*/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;

	/*
	* Default weapon
	*/

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY()
	class AFUDGameMode* FUDGameMode;


public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAOYaw() const { return AO_Yaw; }
	FORCEINLINE float GetAOPitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;

	FVector GetEndHitTarget() const;

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }

	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGamePlay; }
	FORCEINLINE UAnimMontage* GetReloadMontage()  const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
};
