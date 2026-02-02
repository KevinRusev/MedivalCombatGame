
#include "StaminaComponent.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxStamina = 100.f;
	CurrentStamina = 100.f;
	RegenRate = 25.f;
	RegenDelay = 0.75f;
	LastConsumedTime = -1000.f;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentStamina >= MaxStamina || RegenRate <= 0.f)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (World->GetTimeSeconds() - LastConsumedTime < RegenDelay)
	{
		return;
	}

	const float OldStamina = CurrentStamina;
	CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + (RegenRate * DeltaTime));
	const float Delta = CurrentStamina - OldStamina;

	if (!FMath::IsNearlyZero(Delta))
	{
		OnStaminaChanged.Broadcast(this, CurrentStamina, Delta, nullptr);
	}
}

bool UStaminaComponent::TryConsume(float Amount, AActor* Instigator)
{
	if (Amount <= 0.f)
	{
		return true;
	}

	if (CurrentStamina < Amount)
	{
		return false;
	}

	ForceConsume(Amount, Instigator);
	return true;
}

float UStaminaComponent::ForceConsume(float Amount, AActor* Instigator)
{
	if (Amount <= 0.f)
	{
		return 0.f;
	}

	const float OldStamina = CurrentStamina;
	CurrentStamina = FMath::Max(0.f, CurrentStamina - Amount);
	const float Consumed = OldStamina - CurrentStamina;

	if (Consumed > 0.f)
	{
		LastConsumedTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
		OnStaminaChanged.Broadcast(this, CurrentStamina, -Consumed, Instigator);
	}

	return Consumed;
}

float UStaminaComponent::Restore(float Amount, AActor* Instigator)
{
	if (Amount <= 0.f)
	{
		return 0.f;
	}

	const float OldStamina = CurrentStamina;
	CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + Amount);
	const float Restored = CurrentStamina - OldStamina;

	if (Restored > 0.f)
	{
		OnStaminaChanged.Broadcast(this, CurrentStamina, Restored, Instigator);
	}

	return Restored;
}

void UStaminaComponent::ResetStamina()
{
	const float Delta = MaxStamina - CurrentStamina;
	CurrentStamina = MaxStamina;
	LastConsumedTime = -1000.f;
	OnStaminaChanged.Broadcast(this, CurrentStamina, Delta, nullptr);
}
