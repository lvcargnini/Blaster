// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
//class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UWidgetComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()
private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/////// 
	/////// ACTIONS
	/////// 
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//TSoftObjectPtr<UInputMappingContext> DefaultMappingContext;
	class UInputMappingContext* DefaultMappingContext;


	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowGrenadeAction;

	
	/////
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;


	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;



	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	void CrouchButtonPressed();
	//void AimButtonPressed();
	//void AimButtonReleased();
	float CalculateSpeed();

	/**
	* Player health
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;


protected:
	

	// To add mapping context
	virtual void BeginPlay();

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	/** 
	* Player shield
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);


	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void Turn(float Value);
	//void EquipButtonPressed();
	void EquipButtonPressed(const FInputActionValue& Value);

	void StartCrouch(const FInputActionValue& Value);
	void StopCrouch(const FInputActionValue& Value);

	void AimButtonPressed(const FInputActionValue& Value);
	void AimButtonReleased(const FInputActionValue& Value);
	
	void FireButtonPressed(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	
	void CalculateAO_Pitch();
	void SimProxiesTurn();

	void AimOffset(float DeltaTime);
	virtual void Jump() override;
	
	void ReloadButtonPressed(const FInputActionValue& Value);
	//void ReloadButtonPressed();

	//void FireButtonPressed();

	//void FireButtonReleased();

	void GrenadeButtonPressed(const FInputActionValue& Value);

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	/**
	* Dissolve effect
	*/

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on the Blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;

	/**
	* Elim bot
	*/

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;


	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);


	/** 
	 * Blaster components
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;
	
	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	/**
	*	Animation Montages
	*/

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;


	void HideCameraIfCharacterClose();

	//Poll for any relevant classes and initialize our HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);
	//void ServerEquipButtonPressed_Implementation();// const FInputActionValue& Value);


	/** 
	* Grenade
	*/
	UPROPERTY(VisibleAnywhere,  Category = Mesh)
	UStaticMeshComponent* AttachedGrenade;

	/** 
	* Default weapon
	*/

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/** 
	 * Hit boxes used for server-side rewind
	 */
 
	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;
 
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
	UBoxComponent* backpack;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* blanket;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;
 

	
public:
	// Sets default values for this character's properties
	ABlasterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	AWeapon* GetEquippedWeapon();


	virtual void PostInitializeComponents() override;
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayHitReactMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();
	

	
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	/*UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();*/

	virtual void OnRep_ReplicatedMovement() override;

	
	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();

	void SpawDefaultWeapon();

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;
	
	FVector GetHitTarget() const;


	void SetupEnhancedInput_IMC();

	void PossessedBy(AController* NewController) override;

	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

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
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }

	//FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	bool IsLocallyReloading();
};
