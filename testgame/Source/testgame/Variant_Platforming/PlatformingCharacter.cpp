
#include "PlatformingCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "TimerManager.h"
#include "Engine/LocalPlayer.h"

APlatformingCharacter::APlatformingCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	bHasWallJumped = false;
	bHasDoubleJumped = false;
	bHasDashed = false;
	bIsDashing = false;

	OnDashMontageEnded.BindUObject(this, &APlatformingCharacter::DashMontageEnded);

	JumpMaxHoldTime = 0.4f;

	JumpMaxCount = 3;

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->GravityScale = 2.5f;
	GetCharacterMovement()->MaxAcceleration = 1500.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->bUseSeparateBrakingFriction = true;

	GetCharacterMovement()->GroundFriction = 4.0f;
	GetCharacterMovement()->MaxWalkSpeed = 750.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2500.0f;
	GetCharacterMovement()->PerchRadiusThreshold = 15.0f;

	GetCharacterMovement()->JumpZVelocity = 350.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 750.0f;
	GetCharacterMovement()->AirControl = 1.0f;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->NavAgentProps.AgentRadius = 42.0;
	GetCharacterMovement()->NavAgentProps.AgentHeight = 192.0;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.0f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 8.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void APlatformingCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	DoMove(MovementVector.X, MovementVector.Y);
}

void APlatformingCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void APlatformingCharacter::Dash()
{
	
	DoDash();
}

void APlatformingCharacter::MultiJump()
{
	
	if(bIsDashing)
		return;

	if (GetCharacterMovement()->IsFalling())
	{

		if (!bHasWallJumped)
		{
			
			FHitResult OutHit;

			const FVector TraceStart = GetActorLocation();
			const FVector TraceEnd = TraceStart + (GetActorForwardVector() * WallJumpTraceDistance);
			const FCollisionShape TraceShape = FCollisionShape::MakeSphere(WallJumpTraceRadius);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);

			if (GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd, FQuat(), ECollisionChannel::ECC_Visibility, TraceShape, QueryParams))
			{
				
				FRotator WallOrientation = OutHit.ImpactNormal.ToOrientationRotator();
				WallOrientation.Pitch = 0.0f;
				WallOrientation.Roll = 0.0f;

				SetActorRotation(WallOrientation);

				const FVector WallJumpImpulse = (OutHit.ImpactNormal * WallJumpBounceImpulse) + (FVector::UpVector * WallJumpVerticalImpulse);

				LaunchCharacter(WallJumpImpulse, true, true);

				SetJumpTrailState(true);

				bHasWallJumped = true;

				GetWorld()->GetTimerManager().SetTimer(WallJumpTimer, this, &APlatformingCharacter::ResetWallJump, DelayBetweenWallJumps, false);
			}
			
			else
			{
				
				if (GetWorld()->GetTimeSeconds() - LastFallTime < MaxCoyoteTime)
				{
					UE_LOG(LogTemp, Warning, TEXT("Coyote Jump"));

					Jump();

					SetJumpTrailState(true);

				} else {

					if (!bHasDoubleJumped)
					{
						bHasDoubleJumped = true;

						Jump();

						SetJumpTrailState(true);
					}

				}

			}
		}

	}
	else
	{
		
		Jump();

		SetJumpTrailState(true);
	}
}

void APlatformingCharacter::ResetWallJump()
{
	
	bHasWallJumped = false;
}

void APlatformingCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		
		if (!bHasWallJumped)
		{
			
			const FRotator Rotation = GetController()->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			AddMovementInput(ForwardDirection, Forward);
			AddMovementInput(RightDirection, Right);
		}
	}
}

void APlatformingCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void APlatformingCharacter::DoDash()
{
	
	if (bHasDashed)
		return;

	bIsDashing = true;
	bHasDashed = true;

	GetCharacterMovement()->GravityScale = 0.0f;

	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	SetJumpTrailState(true);

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float MontageLength = AnimInstance->Montage_Play(DashMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);

		if (MontageLength > 0.0f)
		{
			AnimInstance->Montage_SetEndDelegate(OnDashMontageEnded, DashMontage);
		}
	}
}

void APlatformingCharacter::DoJumpStart()
{
	
	MultiJump();
}

void APlatformingCharacter::DoJumpEnd()
{
	
	StopJumping();
}

void APlatformingCharacter::DashMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	
	if (bInterrupted)
	{
		EndDash();
	}
}

void APlatformingCharacter::EndDash()
{
	
	GetCharacterMovement()->GravityScale = 2.5f;

	bIsDashing = false;

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		
		bHasDashed = false;

		SetJumpTrailState(false);
	}
}

bool APlatformingCharacter::HasDoubleJumped() const
{
	return bHasDoubleJumped;
}

bool APlatformingCharacter::HasWallJumped() const
{
	return bHasWallJumped;
}

void APlatformingCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(WallJumpTimer);
}

void APlatformingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlatformingCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlatformingCharacter::DoJumpEnd);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformingCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APlatformingCharacter::Look);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlatformingCharacter::Look);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlatformingCharacter::Dash);
	}
}

void APlatformingCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bHasDoubleJumped = false;
	bHasDashed = false;

	SetJumpTrailState(false);
}

void APlatformingCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode )
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		
		LastFallTime = GetWorld()->GetTimeSeconds();
	}
}
