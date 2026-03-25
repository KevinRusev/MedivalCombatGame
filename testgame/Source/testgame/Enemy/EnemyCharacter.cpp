
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->bUseControllerDesiredRotation = false;
		Movement->RotationRate = FRotator(0.f, 540.f, 0.f);
		Movement->bRequestedMoveUseAcceleration = true;
		Movement->MinAnalogWalkSpeed = 0.f;
		Movement->BrakingDecelerationWalking = 2000.f;
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	}

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.f);
	PawnSensingComp->SightRadius = 2000.f;
	PawnSensingComp->HearingThreshold = 600.f;
	PawnSensingComp->LOSHearingThreshold = 1200.f;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	AttackRange = 200.f;
	AttackDamage = 20.f;
	AttackCooldown = 1.5f;
	ChaseSpeed = 450.f;
	PatrolSpeed = 150.f;
	LastAttackTime = -AttackCooldown;
	bIsAttacking = false;
	bIsHitReacting = false;
	bIsDead = false;
	bAttackUsedSingleNode = false;
	TargetActor = nullptr;
	DeathLifespan = 5.f;
	HitStaggerDuration = 0.4f;
	AttackDamageTime = 0.25f;
	
	bForceDefaultAnimBP = false;

	static ConstructorHelpers::FClassFinder<UAnimInstance> UnarmedAnimBPClass(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"));
	if (UnarmedAnimBPClass.Succeeded())
	{
		DefaultAnimInstanceClass = UnarmedAnimBPClass.Class;
		if (GetMesh())
		{
			GetMesh()->SetAnimInstanceClass(UnarmedAnimBPClass.Class);
		}
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> HitReactFinder(TEXT("/Game/Characters/Mannequins/Anims/Rifle/HitReact/MM_HitReact_Front_Med_01"));
	if (HitReactFinder.Succeeded())
	{
		HitReactAnim = HitReactFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> WhisperHit1(TEXT("/Game/Whisper/Animations/Anim_Whisper_Taking_Damage1"));
	if (WhisperHit1.Succeeded())
	{
		HitReactAnim = WhisperHit1.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> WhisperAttack1(TEXT("/Game/Whisper/Animations/Anim_Whisper_Attack1"));
	if (WhisperAttack1.Succeeded())
	{
		AttackAnim = WhisperAttack1.Object;
		AttackDamageTime = 0.30f;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> DeathFinder(TEXT("/Game/Characters/Mannequins/Anims/Death/MM_Death_Front_01"));
	if (DeathFinder.Succeeded())
	{
		DeathAnim = DeathFinder.Object;
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		CachedAnimInstanceClass = GetMesh()->GetAnimClass();
	}

	if (bForceDefaultAnimBP && DefaultAnimInstanceClass && GetMesh() && GetMesh()->GetAnimClass() == nullptr)
	{
		GetMesh()->SetAnimInstanceClass(DefaultAnimInstanceClass);
		CachedAnimInstanceClass = DefaultAnimInstanceClass;
	}

	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSeePawn);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &AEnemyCharacter::OnHearNoise);
	}

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyCharacter::HandleDeath);
		HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyCharacter::HandleHealthChanged);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	}
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead)
	{
		return;
	}

	if (TargetActor && !TargetActor->IsActorBeingDestroyed() && IsTargetInRange() && CanAttack())
	{
		Attack();
	}

	if (TargetActor && !TargetActor->IsActorBeingDestroyed())
	{
		const FVector Velocity = GetVelocity();
		if (Velocity.SizeSquared2D() < FMath::Square(20.f))
		{
			const FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
			if (!ToTarget.IsNearlyZero(1.f))
			{
				const FRotator TargetRot(0.f, ToTarget.Rotation().Yaw, 0.f);
				const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 8.f);
				SetActorRotation(NewRot);
			}
		}
	}
}

float AEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float BaseDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent && DamageAmount > 0.f && !bIsDead)
	{
		return HealthComponent->ReceiveDamage(DamageAmount, DamageCauser);
	}

	return BaseDamage;
}

void AEnemyCharacter::OnSeePawn(APawn* Pawn)
{
	if (bIsDead || !Pawn)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && Pawn == PC->GetPawn())
	{
		TargetActor = Pawn;

		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		}
	}
}

void AEnemyCharacter::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (bIsDead || !NoiseInstigator)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && NoiseInstigator == PC->GetPawn() && !TargetActor)
	{
		TargetActor = NoiseInstigator;
	}
}

void AEnemyCharacter::Attack()
{
	if (!CanAttack())
	{
		return;
	}

	bIsAttacking = true;
	bAttackUsedSingleNode = false;
	LastAttackTime = GetWorld()->GetTimeSeconds();

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UPathFollowingComponent* PFC = AIC->GetPathFollowingComponent())
		{
			PFC->PauseMove(FAIRequestID::CurrentRequest);
		}
	}

	float AttackLen = 0.5f;
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->bPauseAnims = false;
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			if (AttackMontage)
			{
				const float PlayedLen = AnimInst->Montage_Play(AttackMontage, 1.0f);
				if (PlayedLen > 0.f)
				{
					AttackLen = PlayedLen;
				}
			}
		}

		if (!AttackMontage && AttackAnim)
		{
			
			MeshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			MeshComp->SetAnimation(AttackAnim);
			MeshComp->PlayAnimation(AttackAnim, false);
			AttackLen = AttackAnim->GetPlayLength();
			bAttackUsedSingleNode = true;
		}
	}

	const float DamageDelay = FMath::Clamp(AttackDamageTime, 0.0f, FMath::Max(0.01f, AttackLen - 0.01f));
	GetWorldTimerManager().SetTimer(AttackDealDamageTimer, this, &AEnemyCharacter::DealDamage, DamageDelay, false);
	GetWorldTimerManager().SetTimer(AttackFinishTimer, this, &AEnemyCharacter::FinishAttack, FMath::Max(0.05f, AttackLen), false);
}

void AEnemyCharacter::FinishAttack()
{
	bIsAttacking = false;

	if (bIsDead)
	{
		return;
	}

	if (bAttackUsedSingleNode)
	{
		RestoreAnimBPIfNeeded();
		bAttackUsedSingleNode = false;
	}

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UPathFollowingComponent* PFC = AIC->GetPathFollowingComponent())
		{
			PFC->ResumeMove(FAIRequestID::CurrentRequest);
		}
	}
}

void AEnemyCharacter::DealDamage()
{
	if (bIsDead || !TargetActor || !IsTargetInRange())
	{
		return;
	}

	FDamageEvent DamageEvent;
	TargetActor->TakeDamage(AttackDamage, DamageEvent, GetController(), this);

	UE_LOG(LogTemp, Log, TEXT("Enemy dealt %f damage to %s"), AttackDamage, *TargetActor->GetName());
}

bool AEnemyCharacter::CanAttack() const
{
	if (bIsDead || bIsAttacking || bIsHitReacting)
	{
		return false;
	}

	const float TimeSinceLastAttack = GetWorld()->GetTimeSeconds() - LastAttackTime;
	return TimeSinceLastAttack >= AttackCooldown;
}

bool AEnemyCharacter::IsTargetInRange() const
{
	if (!TargetActor)
	{
		return false;
	}

	const float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
	return Distance <= AttackRange;
}

void AEnemyCharacter::HandleHealthChanged(UHealthComponent* , float , float Delta, AActor* DamageInstigator)
{
	if (bIsDead)
	{
		return;
	}

	if (Delta >= 0.f)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[Enemy] Took damage Delta=%.2f HitReactMontage=%s HitReactAnim=%s"),
		Delta,
		HitReactMontage ? *HitReactMontage->GetName() : TEXT("None"),
		HitReactAnim ? *HitReactAnim->GetName() : TEXT("None"));

	if (!TargetActor && DamageInstigator)
	{
		TargetActor = DamageInstigator;

		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		}
	}

	if (bIsAttacking)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(AttackDealDamageTimer);
			World->GetTimerManager().ClearTimer(AttackFinishTimer);
		}
		bIsAttacking = false;
	}

	bIsHitReacting = true;
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UPathFollowingComponent* PFC = AIC->GetPathFollowingComponent())
		{
			PFC->PauseMove(FAIRequestID::CurrentRequest);
		}
	}

	PlayHitReactAnim();

	float HoldTime = FMath::Max(0.05f, HitStaggerDuration);
	if (HitReactAnim)
	{
		HoldTime = FMath::Max(HoldTime, HitReactAnim->GetPlayLength());
	}
	GetWorldTimerManager().SetTimer(HitStaggerTimer, this, &AEnemyCharacter::FinishHitStagger, HoldTime, false);
}

void AEnemyCharacter::FinishHitStagger()
{
	bIsHitReacting = false;

	if (bIsDead)
	{
		return;
	}

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UPathFollowingComponent* PFC = AIC->GetPathFollowingComponent())
		{
			PFC->ResumeMove(FAIRequestID::CurrentRequest);
		}
	}
}

void AEnemyCharacter::PlayHitReactAnim()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return;
	}
	if (!MeshComp->GetSkeletalMeshAsset())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] No skeletal mesh asset; can't play hit react."));
		return;
	}

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	const bool bHasAnimInstance = (AnimInst != nullptr);

	if (HitReactMontage)
	{
		if (bHasAnimInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("[Enemy] Playing HitReactMontage via Montage_Play (%s)"), *HitReactMontage->GetName());
			AnimInst->Montage_Play(HitReactMontage, 1.0f);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Enemy] No AnimInstance; can't play HitReactMontage."));
		}
	}
	else if (HitReactAnim)
	{
		if (HitReactAnim->GetSkeleton() != nullptr && MeshComp->GetSkeletalMeshAsset()->GetSkeleton() != nullptr &&
			HitReactAnim->GetSkeleton() != MeshComp->GetSkeletalMeshAsset()->GetSkeleton())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Enemy] HitReactAnim skeleton mismatch: Anim=%s Mesh=%s"),
				*GetNameSafe(HitReactAnim->GetSkeleton()),
				*GetNameSafe(MeshComp->GetSkeletalMeshAsset()->GetSkeleton()));
		}

		UE_LOG(LogTemp, Log, TEXT("[Enemy] Forcing single-node hit react (%s)"), *HitReactAnim->GetName());
		MeshComp->bPauseAnims = false;
		MeshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		MeshComp->SetAnimation(HitReactAnim);
		MeshComp->PlayAnimation(HitReactAnim, false);

		const float RestoreDelay = FMath::Max3(0.05f, HitStaggerDuration, HitReactAnim->GetPlayLength());
		GetWorldTimerManager().SetTimer(RestoreAnimBPTimer, this, &AEnemyCharacter::RestoreAnimBPIfNeeded, RestoreDelay, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] No hit react assets set."));
	}
}

void AEnemyCharacter::RestoreAnimBPIfNeeded()
{
	if (bIsDead)
	{
		return;
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (CachedAnimInstanceClass)
		{
			MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			MeshComp->SetAnimInstanceClass(CachedAnimInstanceClass);
		}
	}
}

bool AEnemyCharacter::PlayDeathAnim()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return false;
	}
	if (!MeshComp->GetSkeletalMeshAsset())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] No skeletal mesh asset; can't play death anim."));
		return false;
	}
	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	const bool bHasAnimInstance = (AnimInst != nullptr);

	if (DeathMontage)
	{
		
		const float PlayedLen = bHasAnimInstance ? AnimInst->Montage_Play(DeathMontage, 1.0f) : 0.f;
		if (PlayedLen > 0.f)
		{
			UE_LOG(LogTemp, Log, TEXT("[Enemy] Playing DeathMontage via Montage_Play (%s), len=%.2fs"), *DeathMontage->GetName(), PlayedLen);
			GetWorldTimerManager().SetTimer(DeathFreezeTimer, this, &AEnemyCharacter::FreezeDeathPose, FMath::Max(0.05f, PlayedLen - 0.02f), false);
			return true;
		}
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] Failed to play DeathMontage (no anim instance or len=0)."));
		return false;
	}
	if (DeathAnim)
	{
		if (DeathAnim->GetSkeleton() != nullptr && MeshComp->GetSkeletalMeshAsset()->GetSkeleton() != nullptr &&
			DeathAnim->GetSkeleton() != MeshComp->GetSkeletalMeshAsset()->GetSkeleton())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Enemy] DeathAnim skeleton mismatch: Anim=%s Mesh=%s"),
				*GetNameSafe(DeathAnim->GetSkeleton()),
				*GetNameSafe(MeshComp->GetSkeletalMeshAsset()->GetSkeleton()));
		}

		UE_LOG(LogTemp, Log, TEXT("[Enemy] Forcing single-node death anim (%s)"), *DeathAnim->GetName());
		MeshComp->bPauseAnims = false;
		MeshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		MeshComp->SetAnimation(DeathAnim);
		MeshComp->PlayAnimation(DeathAnim, false);
		const float PlayedLen = DeathAnim->GetPlayLength();

		if (PlayedLen > 0.f)
		{
			UE_LOG(LogTemp, Log, TEXT("[Enemy] Death anim play len=%.2fs; scheduling FreezeDeathPose"), PlayedLen);
			GetWorldTimerManager().SetTimer(DeathFreezeTimer, this, &AEnemyCharacter::FreezeDeathPose, FMath::Max(0.05f, PlayedLen - 0.02f), false);
			return true;
		}
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] Death anim play length was 0; nothing played."));
		return false;
	}
	return false;
}

void AEnemyCharacter::FreezeDeathPose()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		
		MeshComp->bPauseAnims = true;
	}
}

void AEnemyCharacter::HandleDeath(UHealthComponent* , AActor* )
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	bIsAttacking = false;
	bIsHitReacting = false;
	TargetActor = nullptr;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AttackDealDamageTimer);
		World->GetTimerManager().ClearTimer(AttackFinishTimer);
		World->GetTimerManager().ClearTimer(HitStaggerTimer);
		World->GetTimerManager().ClearTimer(DeathFreezeTimer);
		World->GetTimerManager().ClearTimer(RestoreAnimBPTimer);
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
	{
		AIController->StopMovement();
		AIController->StopChasing();
		AIController->UnPossess();
	}

	if (PawnSensingComp)
	{
		PawnSensingComp->SetSensingUpdatesEnabled(false);
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (!PlayDeathAnim())
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
			MeshComp->SetSimulatePhysics(true);
		}
	}

	if (UWorld* World = GetWorld())
	{
		FTimerHandle DestroyTimer;
		World->GetTimerManager().SetTimer(DestroyTimer, this, &AEnemyCharacter::DestroyAfterDeath, FMath::Max(0.1f, DeathLifespan), false);
	}

	UE_LOG(LogTemp, Log, TEXT("Enemy %s died."), *GetName());
}

void AEnemyCharacter::DestroyAfterDeath()
{
	Destroy();
}
