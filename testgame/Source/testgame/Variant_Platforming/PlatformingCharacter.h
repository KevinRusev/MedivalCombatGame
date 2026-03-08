
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "PlatformingCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class UAnimMontage;

UCLASS(abstract)
class APlatformingCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* DashAction;

public:

	APlatformingCharacter();

protected:

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void Dash();

	void MultiJump();

	void ResetWallJump();

public:

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoDash();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

protected:

	void DashMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintImplementableEvent, Category="Platforming")
	void SetJumpTrailState(bool bEnabled);

public:

	void EndDash();

public:

	UFUNCTION(BlueprintPure, Category="Platforming")
	bool HasDoubleJumped() const;

	UFUNCTION(BlueprintPure, Category="Platforming")
	bool HasWallJumped() const;

public:
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

protected:

	uint8 bHasWallJumped : 1;
	uint8 bHasDoubleJumped : 1;
	uint8 bHasDashed : 1;
	uint8 bIsDashing : 1;

	FTimerHandle WallJumpTimer;

	FOnMontageEnded OnDashMontageEnded;

	UPROPERTY(EditAnywhere, Category="Wall Jump", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm"))
	float WallJumpTraceDistance = 50.0f;

	UPROPERTY(EditAnywhere, Category="Wall Jump", meta = (ClampMin = 0, ClampMax = 100, Units = "cm"))
	float WallJumpTraceRadius = 25.0f;

	UPROPERTY(EditAnywhere, Category="Wall Jump", meta = (ClampMin = 0, ClampMax = 10000, Units = "cm/s"))
	float WallJumpBounceImpulse = 800.0f;

	UPROPERTY(EditAnywhere, Category="Wall Jump", meta = (ClampMin = 0, ClampMax = 10000, Units = "cm/s"))
	float WallJumpVerticalImpulse = 900.0f;

	UPROPERTY(EditAnywhere, Category="Wall Jump", meta = (ClampMin = 0, ClampMax = 5, Units = "s"))
	float DelayBetweenWallJumps = 0.1f;

	UPROPERTY(EditAnywhere, Category="Dash")
	UAnimMontage* DashMontage;

	float LastFallTime = 0.0f;

	UPROPERTY(EditAnywhere, Category="Coyote Time", meta = (ClampMin = 0, ClampMax = 5, Units = "s"))
	float MaxCoyoteTime = 0.16f;

public:
	
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
