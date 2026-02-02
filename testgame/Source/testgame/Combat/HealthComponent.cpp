
#include "HealthComponent.h"
#include "testgame.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.f;
	CurrentHealth = 100.f;
	InvulnerabilityDuration = 0.25f;
	bIsDead = false;
	LastDamagedTime = -1000.f;
	InvulnerableUntilTime = -1000.f;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	bIsDead = false;
}

float UHealthComponent::ReceiveDamage(float DamageAmount, AActor* DamageInstigator)
{
	if (bIsDead || DamageAmount <= 0.f)
	{
		return 0.f;
	}

	if (IsInvulnerable())
	{
		return 0.f;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Max(0.f, CurrentHealth - DamageAmount);
	const float ActualDamage = OldHealth - CurrentHealth;

	LastDamagedTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	OnHealthChanged.Broadcast(this, CurrentHealth, -ActualDamage, DamageInstigator);

	if (CurrentHealth <= 0.f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast(this, DamageInstigator);
	}

	return ActualDamage;
}

float UHealthComponent::Heal(float HealAmount, AActor* Healer)
{
	if (bIsDead || HealAmount <= 0.f)
	{
		return 0.f;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
	const float ActualHeal = CurrentHealth - OldHealth;

	if (ActualHeal > 0.f)
	{
		OnHealthChanged.Broadcast(this, CurrentHealth, ActualHeal, Healer);
	}

	return ActualHeal;
}

void UHealthComponent::Kill(AActor* Killer)
{
	if (bIsDead)
	{
		return;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = 0.f;
	bIsDead = true;

	OnHealthChanged.Broadcast(this, CurrentHealth, -OldHealth, Killer);
	OnDeath.Broadcast(this, Killer);
}

void UHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
	bIsDead = false;
	LastDamagedTime = -1000.f;
	OnHealthChanged.Broadcast(this, CurrentHealth, MaxHealth, nullptr);
}

bool UHealthComponent::IsInvulnerable() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float Now = World->GetTimeSeconds();

	if (Now < InvulnerableUntilTime)
	{
		return true;
	}

	if (InvulnerabilityDuration > 0.f && (Now - LastDamagedTime) < InvulnerabilityDuration)
	{
		return true;
	}

	return false;
}

void UHealthComponent::SetInvulnerableFor(float Duration)
{
	if (Duration <= 0.f || !GetWorld())
	{
		return;
	}
	InvulnerableUntilTime = GetWorld()->GetTimeSeconds() + Duration;
}
