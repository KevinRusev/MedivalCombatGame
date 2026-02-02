
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnStaminaChanged, UStaminaComponent*, StaminaComp, float, NewStamina, float, Delta, AActor*, Instigator);

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class TESTGAME_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool TryConsume(float Amount, AActor* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float ForceConsume(float Amount, AActor* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float Restore(float Amount, AActor* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ResetStamina();

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetCurrentStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaxStamina() const { return MaxStamina; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetStaminaPercent() const { return MaxStamina > 0.f ? FMath::Clamp(CurrentStamina / MaxStamina, 0.f, 1.f) : 0.f; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool HasEnough(float Cost) const { return CurrentStamina >= Cost; }

public:
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaChanged OnStaminaChanged;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "1.0"))
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "0.0"))
	float RegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "0.0", Units = "s"))
	float RegenDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina;

private:
	float LastConsumedTime;
};
