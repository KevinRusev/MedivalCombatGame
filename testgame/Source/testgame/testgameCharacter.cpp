
#include "testgameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/DamageEvents.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InventoryComponent.h"
#include "InteractionComponent.h"
#include "HealthComponent.h"
#include "StaminaComponent.h"
#include "testgameGameplayHUD.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "testgame.h"

AtestgameCharacter::AtestgameCharacter()
{
	
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->SetInventoryComponent(InventoryComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));

	HeldItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldItemMesh"));
	HeldItemMesh->SetupAttachment(GetMesh());
	HeldItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeldItemMesh->SetVisibility(false);

	HeldItemSocketName = FName("hand_r");

	HeldItemActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("HeldItemActor"));
	HeldItemActor->SetupAttachment(GetMesh(), HeldItemSocketName);
	HeldItemActor->SetVisibility(false);

	MeleeDamage = 25.f;
	MeleeRange = 160.f;
	MeleeRadius = 75.f;
	AttackCooldown = 0.15f;
	AttackHitDelay = 0.2f;
	AttackDuration = 0.55f;
	AttackStaminaCost = 15.f;
	
	MaxComboHits = 2;
	ComboInputWindow = 0.55f;
	ComboResetDelay = 1.2f;
	ComboDamageMultipliers = { 1.0f, 1.15f };
	bRequiresWeaponToAttack = true;
	bDebugDrawAttackTrace = false;

	DodgeLaunchSpeed = 1400.f;
	DodgeDuration = 0.87f;
	DodgeIFrameDuration = 0.45f;
	BackstepDuration = 0.60f;
	BackstepLaunchSpeed = 900.f;
	DodgeStaminaCost = 25.f;

	bIsAttacking = false;
	bIsDodging = false;
	bIsBackstepping = false;
	bComboQueued = false;
	ComboIndex = 0;
	LastAttackTime = -1000.f;
	DodgeStartTime = -1000.f;
	CurrentDodgeDuration = DodgeDuration;
	CurrentDodgeLaunchSpeed = DodgeLaunchSpeed;
	DodgeDirection = FVector::ZeroVector;
	SavedGroundFriction = 8.f;
	SavedBrakingDecelWalking = 2000.f;

	static ConstructorHelpers::FObjectFinder<UAnimSequence> HitReactFinder(TEXT("/Game/Characters/Mannequins/Anims/Rifle/HitReact/MM_HitReact_Front_Med_01"));
	if (HitReactFinder.Succeeded())
	{
		HitReactAnim = HitReactFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> DeathFinder(TEXT("/Game/Characters/Mannequins/Anims/Death/MM_Death_Front_01"));
	if (DeathFinder.Succeeded())
	{
		DeathAnim = DeathFinder.Object;
	}

}

void AtestgameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AtestgameCharacter::HandleDeath);
		HealthComponent->OnHealthChanged.AddDynamic(this, &AtestgameCharacter::HandleHealthChanged);
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		SavedGroundFriction = Movement->GroundFriction;
		SavedBrakingDecelWalking = Movement->BrakingDecelerationWalking;
	}

	EnsureGameplayHUDForLocalPlayer();
}

void AtestgameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsDodging)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float Elapsed = World->GetTimeSeconds() - DodgeStartTime;
	const float Alpha = CurrentDodgeDuration > 0.f
		? FMath::Clamp(Elapsed / CurrentDodgeDuration, 0.f, 1.f)
		: 1.f;

	const float SpeedFactor = FMath::Pow(1.f - Alpha, 2.f);
	const FVector TargetVelXY = DodgeDirection * CurrentDodgeLaunchSpeed * SpeedFactor;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		FVector V = Movement->Velocity;
		V.X = TargetVelXY.X;
		V.Y = TargetVelXY.Y;
		Movement->Velocity = V;
	}
}

void AtestgameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	EnsureGameplayHUDForLocalPlayer();
}

void AtestgameCharacter::EnsureGameplayHUDForLocalPlayer()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->IsLocalPlayerController())
	{
		return;
	}

	if (PC->GetHUD() == nullptr || !PC->GetHUD()->IsA(AtestgameGameplayHUD::StaticClass()))
	{
		PC->ClientSetHUD(AtestgameGameplayHUD::StaticClass());
	}
}

void AtestgameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AtestgameCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AtestgameCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AtestgameCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AtestgameCharacter::DoInteract);

		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AtestgameCharacter::AttackInput);
		}

		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AtestgameCharacter::DodgeInput);
		}
	}
	else
	{
		UE_LOG(Logtestgame, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	if (PlayerInputComponent)
	{
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AtestgameCharacter::AttackInput);
		PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &AtestgameCharacter::DodgeInput);
	}
}

void AtestgameCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AtestgameCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AtestgameCharacter::DoMove(float Right, float Forward)
{
	
	if (bIsDodging)
	{
		return;
	}

	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AtestgameCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AtestgameCharacter::DoJumpStart()
{
	Jump();
}

void AtestgameCharacter::DoJumpEnd()
{
	StopJumping();
}

void AtestgameCharacter::DoInteract()
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}

void AtestgameCharacter::AttackInput()
{
	DoAttack();
}

void AtestgameCharacter::DodgeInput()
{
	DoDodge();
}

void AtestgameCharacter::DoAttack()
{
	if (!HealthComponent || HealthComponent->IsDead())
	{
		return;
	}

	if (bIsDodging)
	{
		return;
	}

	if (bIsAttacking)
	{
		const UWorld* World = GetWorld();
		if (World && (ComboIndex + 1) < MaxComboHits)
		{
			
			bComboQueued = true;
			UE_LOG(Logtestgame, Verbose, TEXT("Combo queue input: ComboIndex=%d MaxComboHits=%d Elapsed=%.3f Queued=%d"),
				ComboIndex, MaxComboHits, (float)(World->GetTimeSeconds() - LastAttackTime), bComboQueued ? 1 : 0);
		}
		else
		{
			UE_LOG(Logtestgame, Verbose, TEXT("Combo queue ignored: ComboIndex=%d MaxComboHits=%d"), ComboIndex, MaxComboHits);
		}
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (World->GetTimeSeconds() - LastAttackTime < AttackCooldown)
	{
		return;
	}

	if (bRequiresWeaponToAttack && !HasItemEquipped())
	{
		UE_LOG(Logtestgame, Verbose, TEXT("Attack blocked: no weapon equipped."));
		return;
	}

	if (StaminaComponent && !StaminaComponent->TryConsume(AttackStaminaCost, this))
	{
		UE_LOG(Logtestgame, Verbose, TEXT("Attack blocked: out of stamina."));
		return;
	}

	PerformAttackSwing();
}

void AtestgameCharacter::PerformAttackSwing()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bIsAttacking = true;
	bComboQueued = false;
	LastAttackTime = World->GetTimeSeconds();
	AttackHitTargetsThisSwing.Reset();

	GetWorldTimerManager().ClearTimer(ComboResetTimer);

	UAnimMontage* MontageToPlay = nullptr;
	if (ComboMontages.IsValidIndex(ComboIndex) && ComboMontages[ComboIndex])
	{
		MontageToPlay = ComboMontages[ComboIndex];
	}
	else if (AttackMontage)
	{
		MontageToPlay = AttackMontage;
	}

	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		if (MontageToPlay)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay, 1.0f);
		}
		else
		{
			UAnimSequenceBase* AnimToPlay = nullptr;
			if (ComboAnims.IsValidIndex(ComboIndex) && ComboAnims[ComboIndex])
			{
				AnimToPlay = ComboAnims[ComboIndex];
			}
			else if (AttackAnim)
			{
				AnimToPlay = AttackAnim;
			}

			if (!AnimToPlay)
			{
				
				UE_LOG(Logtestgame, Warning, TEXT("Attack anim missing: ComboIndex=%d (ComboAnims.Num=%d)"), ComboIndex, ComboAnims.Num());
			}
			
			else if (UAnimMontage* AsMontage = Cast<UAnimMontage>(AnimToPlay))
			{
				GetMesh()->GetAnimInstance()->Montage_Play(AsMontage, 1.0f);
			}
			else
			{
				GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(
					AnimToPlay,
					FName("DefaultSlot"),
					0.08f,
					0.12f,
					1.0f,
					1,
					-1.0f,
					0.0f
				);
			}
		}
	}

	float StepHitDelay = AttackHitDelay;
	if (ComboHitDelays.IsValidIndex(ComboIndex) && ComboHitDelays[ComboIndex] > 0.f)
	{
		StepHitDelay = FMath::Max(0.001f, ComboHitDelays[ComboIndex]);
	}

	float StepDuration = AttackDuration;
	if (ComboAttackDurations.IsValidIndex(ComboIndex) && ComboAttackDurations[ComboIndex] > 0.f)
	{
		StepDuration = FMath::Max(StepHitDelay + 0.01f, ComboAttackDurations[ComboIndex]);
	}
	StepDuration = FMath::Max(StepHitDelay + 0.01f, StepDuration);

	UE_LOG(Logtestgame, Verbose, TEXT("Attack swing start: ComboIndex=%d MaxComboHits=%d StepHitDelay=%.3f StepDuration=%.3f Cooldown=%.3f"),
		ComboIndex, MaxComboHits, StepHitDelay, StepDuration, AttackCooldown);

	GetWorldTimerManager().SetTimer(AttackHitTimer, this, &AtestgameCharacter::PerformAttackTrace, StepHitDelay, false);
	GetWorldTimerManager().SetTimer(AttackFinishTimer, this, &AtestgameCharacter::FinishAttack, StepDuration, false);
}

bool AtestgameCharacter::DoDodge()
{
	if (!HealthComponent || HealthComponent->IsDead())
	{
		return false;
	}

	if (bIsDodging)
	{
		return false;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	if (bIsAttacking)
	{
		GetWorldTimerManager().ClearTimer(AttackHitTimer);
		GetWorldTimerManager().ClearTimer(AttackFinishTimer);
		bIsAttacking = false;
		bComboQueued = false;
		AttackHitTargetsThisSwing.Reset();
	}

	if (StaminaComponent && !StaminaComponent->TryConsume(DodgeStaminaCost, this))
	{
		UE_LOG(Logtestgame, Verbose, TEXT("Dodge blocked: out of stamina."));
		return false;
	}

	FVector InputDir = GetLastMovementInputVector();
	const bool bHasInput = !InputDir.IsNearlyZero();

	if (bHasInput)
	{
		DodgeDirection = InputDir.GetSafeNormal2D();
		bIsBackstepping = false;
		CurrentDodgeDuration = DodgeDuration;
		CurrentDodgeLaunchSpeed = DodgeLaunchSpeed;
	}
	else
	{
		
		DodgeDirection = -GetActorForwardVector().GetSafeNormal2D();
		bIsBackstepping = true;
		CurrentDodgeDuration = BackstepDuration;
		CurrentDodgeLaunchSpeed = BackstepLaunchSpeed;
	}

	bIsDodging = true;
	DodgeStartTime = World->GetTimeSeconds();

	if (!DodgeDirection.IsNearlyZero())
	{
		const FVector FaceDir = bIsBackstepping ? -DodgeDirection : DodgeDirection;
		const FRotator FaceRot(0.f, FaceDir.Rotation().Yaw, 0.f);
		SetActorRotation(FaceRot);
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->Velocity.X = DodgeDirection.X * CurrentDodgeLaunchSpeed;
		Movement->Velocity.Y = DodgeDirection.Y * CurrentDodgeLaunchSpeed;
		Movement->GroundFriction = 0.f;
		Movement->BrakingDecelerationWalking = 0.f;
	}

	if (HealthComponent && DodgeIFrameDuration > 0.f)
	{
		HealthComponent->SetInvulnerableFor(DodgeIFrameDuration);
	}

	if (UAnimMontage* MontageToPlay = (bIsBackstepping && BackstepMontage) ? BackstepMontage : DodgeMontage)
	{
		if (GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay, 1.0f);
		}
	}

	GetWorldTimerManager().SetTimer(DodgeFinishTimer, this, &AtestgameCharacter::FinishDodge, FMath::Max(0.05f, CurrentDodgeDuration), false);

	UE_LOG(Logtestgame, Log, TEXT("%s dur=%.2fs iframes=%.2fs"),
		bIsBackstepping ? TEXT("Backstep") : TEXT("Dodge roll"),
		CurrentDodgeDuration, DodgeIFrameDuration);
	return true;
}

void AtestgameCharacter::FinishDodge()
{
	bIsDodging = false;
	bIsBackstepping = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->GroundFriction = SavedGroundFriction;
		Movement->BrakingDecelerationWalking = SavedBrakingDecelWalking;
		
	}
}

void AtestgameCharacter::PerformAttackTrace()
{
	if (!bIsAttacking)
	{
		return;
	}

	const FVector TraceStart = GetActorLocation() + GetActorForwardVector() * (GetCapsuleComponent()->GetScaledCapsuleRadius());
	const FVector TraceEnd = TraceStart + GetActorForwardVector() * MeleeRange;

	TArray<FHitResult> Hits;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	const bool bHit = GetWorld()->SweepMultiByObjectType(
		Hits,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(MeleeRadius),
		QueryParams
	);

	if (bDebugDrawAttackTrace)
	{
		DrawDebugSphere(GetWorld(), TraceStart, MeleeRadius, 12, FColor::Yellow, false, 0.5f);
		DrawDebugSphere(GetWorld(), TraceEnd, MeleeRadius, 12, FColor::Red, false, 0.5f);
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Orange, false, 0.5f, 0, 2.0f);
	}

	if (!bHit)
	{
		return;
	}

	const float Multiplier = ComboDamageMultipliers.IsValidIndex(ComboIndex)
		? ComboDamageMultipliers[ComboIndex]
		: 1.f;
	const float SwingDamage = MeleeDamage * Multiplier;

	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == this)
		{
			continue;
		}

		const TWeakObjectPtr<AActor> WeakActor(HitActor);
		if (AttackHitTargetsThisSwing.Contains(WeakActor))
		{
			continue;
		}
		AttackHitTargetsThisSwing.Add(WeakActor);

		FDamageEvent DamageEvent;
		HitActor->TakeDamage(SwingDamage, DamageEvent, GetController(), this);

		UE_LOG(Logtestgame, Log, TEXT("Player melee combo %d hit %s for %.1f"), ComboIndex + 1, *HitActor->GetName(), SwingDamage);
	}
}

void AtestgameCharacter::FinishAttack()
{
	bIsAttacking = false;
	AttackHitTargetsThisSwing.Reset();

	if (bComboQueued && (ComboIndex + 1) < MaxComboHits)
	{
		UE_LOG(Logtestgame, Verbose, TEXT("Combo chain attempt: ComboIndex=%d -> %d (Max=%d)"),
			ComboIndex, ComboIndex + 1, MaxComboHits);
		if (!StaminaComponent || StaminaComponent->TryConsume(AttackStaminaCost, this))
		{
			++ComboIndex;
			bComboQueued = false;
			PerformAttackSwing();
			return;
		}

		UE_LOG(Logtestgame, Warning, TEXT("Combo chain blocked: not enough stamina for hit %d (Cost=%.2f)"),
			ComboIndex + 2, AttackStaminaCost);
		bComboQueued = false;
	}
	else
	{
		UE_LOG(Logtestgame, Verbose, TEXT("Combo chain end: Queued=%d ComboIndex=%d Max=%d"),
			bComboQueued ? 1 : 0, ComboIndex, MaxComboHits);
	}

	ComboIndex = 0;

	GetWorldTimerManager().SetTimer(ComboResetTimer, this, &AtestgameCharacter::ResetCombo, FMath::Max(0.05f, ComboResetDelay), false);
}

void AtestgameCharacter::ResetCombo()
{
	ComboIndex = 0;
	bComboQueued = false;
}

float AtestgameCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float BaseDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent && DamageAmount > 0.f)
	{
		return HealthComponent->ReceiveDamage(DamageAmount, DamageCauser);
	}

	return BaseDamage;
}

void AtestgameCharacter::HandleHealthChanged(UHealthComponent* , float , float Delta, AActor* )
{
	if (Delta >= 0.f)
	{
		return;
	}
	if (HealthComponent && HealthComponent->IsDead())
	{
		
		return;
	}
	if (bIsDodging)
	{
		
		return;
	}
	PlayHitReactAnim();
}

void AtestgameCharacter::PlayHitReactAnim()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return;
	}
	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst)
	{
		return;
	}

	if (HitReactMontage)
	{
		AnimInst->Montage_Play(HitReactMontage, 1.0f);
	}
	else if (HitReactAnim)
	{
		
		if (UAnimMontage* AsMontage = Cast<UAnimMontage>(HitReactAnim))
		{
			AnimInst->Montage_Play(AsMontage, 1.0f);
		}
		else
		{
			AnimInst->PlaySlotAnimationAsDynamicMontage(HitReactAnim, FName("DefaultSlot"), 0.1f, 0.2f, 1.0f, 1, -1.0f, 0.0f);
		}
	}
}

bool AtestgameCharacter::PlayDeathAnim()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return false;
	}
	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst)
	{
		return false;
	}

	if (DeathMontage)
	{
		AnimInst->Montage_Play(DeathMontage, 1.0f);
		return true;
	}
	if (DeathAnim)
	{
		if (UAnimMontage* AsMontage = Cast<UAnimMontage>(DeathAnim))
		{
			AnimInst->Montage_Play(AsMontage, 1.0f);
		}
		else
		{
			AnimInst->PlaySlotAnimationAsDynamicMontage(DeathAnim, FName("DefaultSlot"), 0.1f, 0.2f, 1.0f, 1, -1.0f, 0.0f);
		}
		return true;
	}
	return false;
}

void AtestgameCharacter::HandleDeath(UHealthComponent* , AActor* )
{
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackHitTimer);
		World->GetTimerManager().ClearTimer(AttackFinishTimer);
		World->GetTimerManager().ClearTimer(ComboResetTimer);
		World->GetTimerManager().ClearTimer(DodgeFinishTimer);
	}
	bIsAttacking = false;
	bIsDodging = false;
	bIsBackstepping = false;
	bComboQueued = false;
	ComboIndex = 0;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->GroundFriction = SavedGroundFriction;
		Movement->BrakingDecelerationWalking = SavedBrakingDecelWalking;
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	if (!PlayDeathAnim())
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
			MeshComp->SetSimulatePhysics(true);
		}
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UE_LOG(Logtestgame, Log, TEXT("Player character died."));
}

void AtestgameCharacter::EquipItem(const FItemData& Item)
{
	if (!Item.IsValid())
	{
		return;
	}

	UnequipItem();

	EquippedItem = Item;

	if (Item.HeldActorClass && HeldItemActor)
	{
		HeldItemActor->SetChildActorClass(Item.HeldActorClass);
		HeldItemActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HeldItemSocketName);
		HeldItemActor->SetVisibility(true);

		if (AActor* ChildActor = HeldItemActor->GetChildActor())
		{
			ChildActor->SetActorEnableCollision(false);
		}

		UE_LOG(Logtestgame, Log, TEXT("Equipped Blueprint item: %s"), *Item.ItemID.ToString());
	}
	else if (Item.HeldMesh && HeldItemMesh)
	{
		HeldItemMesh->SetStaticMesh(Item.HeldMesh);
		HeldItemMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HeldItemSocketName);
		HeldItemMesh->SetVisibility(true);

		UE_LOG(Logtestgame, Log, TEXT("Equipped mesh item: %s"), *Item.ItemID.ToString());
	}
}

void AtestgameCharacter::UnequipItem()
{
	EquippedItem = FItemData();

	if (HeldItemMesh)
	{
		HeldItemMesh->SetVisibility(false);
		HeldItemMesh->SetStaticMesh(nullptr);
	}

	if (HeldItemActor)
	{
		HeldItemActor->SetVisibility(false);
		HeldItemActor->SetChildActorClass(nullptr);
	}
}
