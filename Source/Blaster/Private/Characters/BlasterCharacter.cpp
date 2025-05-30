	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 // Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "Characters/BlasterAnimInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Weapon.h"
#include "Weapons/WeaponTypes.h"
#include "BlasterComponents/CombatComponent.h"
#include "BlasterComponents/BuffComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Blaster.h"
#include "PlayerController/BlasterPlayerController.h"
#include "GameMode/BlasterGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayerState/BlasterPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "UObject/NameTypes.h"
#include "Components/BoxComponent.h"
#include "BlasterComponents/LagCompensationComponent.h"


ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);
	
	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));
 

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	//NetUpdateFrequency = 66.f;
	SetNetUpdateFrequency(66.f);
	//MinNetUpdateFrequency = 33.f;
	SetMinNetUpdateFrequency(33.f);


	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
	
	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("LeftGrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//InputMapping = CreateDefaultSubobject<UInputMappingContext>(TEXT("OverheadWidget"));

	
 	/** 
 	* Hit boxes for server-side rewind
 	*/
 
	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
 	head->SetupAttachment(GetMesh(), FName("head"));
 	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("head"), head);
 
 	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
 	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
 	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("pelvis"), pelvis);
 
 	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
 	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
 	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("spine_02"), spine_02);
 
 	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
 	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
 	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("spine_03"), spine_03);
 
 	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
 	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
 	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);
 
 	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
 	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
 	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);
 
 	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
 	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
 	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);
 
 	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
 	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
 	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);
 
 	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
 	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
 	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("hand_l"), hand_l);
 
 	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
 	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
 	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("hand_r"), hand_r);
 
 	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
 	blanket->SetupAttachment(GetMesh(), FName("backpack"));
 	blanket->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("blanket"), blanket);
 
 	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
 	backpack->SetupAttachment(GetMesh(), FName("backpack"));
 	backpack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("backpack"), backpack);
 
 	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
 	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
 	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);
 
 	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
 	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
 	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);
 
 	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
 	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
 	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("calf_l"), calf_l);
 
 	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
 	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
 	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("calf_r"), calf_r);
 
 	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
 	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
 	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("foot_l"), foot_l);
 
 	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
 	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
 	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 	HitCollisionBoxes.Add(FName("foot_r"), foot_r);
	
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, Shield);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::Elim()
{
	DropOrDestroyWeapons();
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ABlasterCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
	}
}

void ABlasterCharacter::MulticastElim_Implementation()
{
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();
	// Start dissolve effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();

	// Disable character movement
	//bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn elim bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation()
		);
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}
	bool bHideSniperScope = IsLocallyControlled() && 
		Combat && 
		Combat->bAiming && 
		Combat->EquippedWeapon && 
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
}

void ABlasterCharacter::ElimTimerFinished()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (BlasterGameMode)
	{
		BlasterGameMode->RequestRespawn(this, Controller);
	}
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGameMode && BlasterGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}
	
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}

	if (!HasAuthority())
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext,0);
			}
		}
	}
	
}

void ABlasterCharacter::SetupEnhancedInput_IMC()
{
	BlasterPlayerController = Cast<ABlasterPlayerController>(GetController());

	if (BlasterPlayerController && DefaultMappingContext)
	{
		UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BlasterPlayerController->GetLocalPlayer());

		if (EISubsystem) EISubsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ABlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
				Subsystem->AddMappingContext(DefaultMappingContext,0);
		}
	}
}


void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}

	HideCameraIfCharacterClose();
	PollInit();
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	/*APlayerController* PlayerController = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());


	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);*/

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		//EnhancedInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);

		
		//PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
		//PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
		//PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
		//PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);

		//EnhancedInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
		//Input->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
		//EnhancedInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
		//EnhancedInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
		//EnhancedInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
		//EnhancedInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);


		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABlasterCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::EquipButtonPressed);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABlasterCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABlasterCharacter::StopCrouch);

		// Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABlasterCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABlasterCharacter::AimButtonReleased);

		//Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABlasterCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterCharacter::FireButtonReleased);

		//Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &ABlasterCharacter::ReloadButtonPressed);
		//EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterCharacter::FireButtonReleased);

		//Fire
		//EnhancedInputComponent->BindAction(SwitchCameraAction, ETriggerEvent::Completed, this, &ADDCharacter::SwitchCamera);
		//EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ADDCharacter::FireButtonReleased);
		//throw Grenade
		EnhancedInputComponent->BindAction(ThrowGrenadeAction, ETriggerEvent::Completed, this, &ABlasterCharacter::GrenadeButtonPressed);
	}

}

void ABlasterCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(
			GetCharacterMovement()->MaxWalkSpeed, 
			GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
			case EWeaponType::EWT_AssaultRifle:
				SectionName = FName("Rifle");
				break;
			case EWeaponType::EWT_RocketLauncher:
				SectionName = FName("Rocketauncher");
				break;
			case EWeaponType::EWT_Pistol:
				SectionName = FName("Pistol");
				break;
			case EWeaponType::EWT_SubmachineGun:
				SectionName = FName("Pistol");
				break;
			case EWeaponType::EWT_Shotgun:
				SectionName = FName("Shotgun");
				break;
			case EWeaponType::EWT_SniperRifle:
				SectionName = FName("SniperRifle");
				break;
			case EWeaponType::EWT_GrenadeLauncher:
				SectionName = FName("GrenadeLauncher");
				break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimmed) return;

	float DamageToHealth = Damage;
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			Shield = 0.f;
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
		}
	}

	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);
	
	UpdateHUDHealth();
	UpdateHUDShield();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (BlasterGameMode)
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}

//void ABlasterCharacter::MoveForward(float Value)
//{
//	if (Controller != nullptr && Value != 0.f)
//	{
//		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
//		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
//		AddMovementInput(Direction, Value);
//	}
//}
//
//void ABlasterCharacter::MoveRight(float Value)
//{
//	if (Controller != nullptr && Value != 0.f)
//	{
//		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
//		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
//		AddMovementInput(Direction, Value);
//	}
//}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("bDisableGameplay %d"), bDisableGameplay);
	//if (bDisableGameplay) return;
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	//if (bDisableGameplay) return;

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

//void ABlasterCharacter::LookUp(float Value)
//{
//	AddControllerPitchInput(Value);
//}

void ABlasterCharacter::EquipButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		ServerEquipButtonPressed();
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapons())
		{
			Combat->SwapWeapons();
		}
	}
}

void ABlasterCharacter::StartCrouch(const FInputActionValue& Value) {
	if (bDisableGameplay) return; 
	if (!bIsCrouched)
	{
		Crouch();
	}
}

void ABlasterCharacter::StopCrouch(const FInputActionValue& Value) {
	if (bIsCrouched)
	{
		UnCrouch();
	}
	
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}
void ABlasterCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->Reload();
	}
}


void ABlasterCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::GrenadeButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

}

void ABlasterCharacter::Jump()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth){
    		PlayHitReactMontage();
	}
}

void ABlasterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::UpdateHUDShield()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void ABlasterCharacter::UpdateHUDAmmo()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController && Combat && Combat->EquippedWeapon)
	{
		BlasterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		BlasterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void ABlasterCharacter::SpawDefaultWeapon() 
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (BlasterGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
		}
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}                         

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

bool ABlasterCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}
