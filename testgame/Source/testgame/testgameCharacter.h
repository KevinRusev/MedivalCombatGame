
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PickupItem.h"
#include "testgameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInventoryComponent;
class UInteractionComponent;
class UChildActorComponent;
class UHealthComponent;
class UStaminaComponent;
class UAnimMontage;
class UAnimSequenceBase;
struct FInputActionValue;

UCLASS(abstract)
class AtestgameCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UInteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* HeldItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* HeldItemActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaminaComponent* StaminaComponent;
	
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
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FName HeldItemSocketName;

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	FItemData EquippedItem;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0"))
	float MeleeDamage;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0", Units = "cm"))
	float MeleeRange;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0", Units = "cm"))
	float MeleeRadius;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0", Units = "s"))
	float AttackCooldown;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0", Units = "s"))
	float AttackHitDelay;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0", Units = "s"))
	float AttackDuration;

	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimSequenceBase* AttackAnim;

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<UAnimMontage*> ComboMontages;

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<UAnimSequenceBase*> ComboAnims;

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<float> ComboHitDelays;

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<float> ComboAttackDurations;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0"))
	float AttackStaminaCost;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "1"))
	int32 MaxComboHits;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ComboInputWindow;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (ClampMin = "0.0", Units = "s"))
	float ComboResetDelay;

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<float> ComboDamageMultipliers;

	UPROPERTY(EditAnywhere, Category="Combat")
	bool bRequiresWeaponToAttack;

	UPROPERTY(EditAnywhere, Category="Combat|Debug")
	bool bDebugDrawAttackTrace;

	UPROPERTY(EditAnywhere, Category="Dodge", meta = (ClampMin = "0", Units = "cm/s"))
	float DodgeLaunchSpeed;

	UPROPERTY(EditAnywhere, Category="Dodge", meta = (ClampMin = "0", Units = "s"))
	float DodgeDuration;

	UPROPERTY(EditAnywhere, Category="Dodge", meta = (ClampMin = "0", Units = "s"))
	float DodgeIFrameDuration;

	UPROPERTY(EditAnywhere, Category="Dodge", meta = (ClampMin = "0", Units = "s"))
	float BackstepDuration;

	UPROPERTY(EditAnywhere, Category="Dodge", meta = (ClampMin = "0", Units = "cm/s"))
	float BackstepLaunchSpeed;

	UPROPERTY(EditAnywhere, Category="Dodge", meta = (ClampMin = "0"))
	float DodgeStaminaCost;

	UPROPERTY(EditAnywhere, Category="Dodge")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category="Dodge")
	UAnimMontage* BackstepMontage;

	UPROPERTY(EditAnywhere, Category="Combat|Anim")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category="Combat|Anim")
	UAnimSequenceBase* HitReactAnim;

	UPROPERTY(EditAnywhere, Category="Combat|Anim")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category="Combat|Anim")
	UAnimSequenceBase* DeathAnim;

public:

	AtestgameCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

protected:

	void EnsureGameplayHUDForLocalPlayer();

protected:

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void AttackInput();

	void DodgeInput();

	void PerformAttackTrace();

	void FinishAttack();

	void ResetCombo();

	void FinishDodge();

	UFUNCTION()
	void HandleDeath(UHealthComponent* HealthComp, AActor* Killer);

	UFUNCTION()
	void HandleHealthChanged(UHealthComponent* HealthComp, float NewHealth, float Delta, AActor* DamageInstigator);

public:

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoInteract();

	UFUNCTION(BlueprintCallable, Category="Combat")
	virtual void DoAttack();

	UFUNCTION(BlueprintCallable, Category="Combat")
	virtual bool DoDodge();

	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsDodging() const { return bIsDodging; }

	UFUNCTION(BlueprintPure, Category="Combat")
	int32 GetComboIndex() const { return ComboIndex; }

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void EquipItem(const FItemData& Item);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void UnequipItem();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool HasItemEquipped() const { return EquippedItem.IsValid(); }

	UFUNCTION(BlueprintCallable, Category="Inventory")
	FItemData GetEquippedItem() const { return EquippedItem; }

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	FORCEINLINE class UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

	FORCEINLINE class UStaticMeshComponent* GetHeldItemMesh() const { return HeldItemMesh; }

	FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	FORCEINLINE class UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }

private:
	
	void PerformAttackSwing();

	void PlayHitReactAnim();

	bool PlayDeathAnim();

	bool bIsAttacking;
	bool bIsDodging;
	bool bIsBackstepping;
	bool bComboQueued;
	int32 ComboIndex;
	float LastAttackTime;
	float DodgeStartTime;
	float CurrentDodgeDuration;
	float CurrentDodgeLaunchSpeed;
	FVector DodgeDirection;

	float SavedGroundFriction;
	float SavedBrakingDecelWalking;

	FTimerHandle AttackHitTimer;
	FTimerHandle AttackFinishTimer;
	FTimerHandle ComboResetTimer;
	FTimerHandle DodgeFinishTimer;
	TSet<TWeakObjectPtr<AActor>> AttackHitTargetsThisSwing;
};
