
#include "testgameGameplayHUD.h"
#include "testgameCharacter.h"
#include "Enemy/EnemyCharacter.h"
#include "Combat/HealthComponent.h"
#include "Combat/StaminaComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

namespace
{
	
	constexpr float EnemyBarMaxDistance = 3500.f;

	constexpr float EnemyBarHeadroom = 25.f;
}

void AtestgameGameplayHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	if (AtestgameCharacter* Character = Cast<AtestgameCharacter>(PC->GetPawn()))
	{
		DrawPlayerHealth(Character);
	}

	DrawEnemyHealthBars(PC);
}

void AtestgameGameplayHUD::DrawPlayerHealth(AtestgameCharacter* Character)
{
	UHealthComponent* Health = Character->GetHealthComponent();
	if (!Health)
	{
		return;
	}

	UStaminaComponent* Stamina = Character->GetStaminaComponent();

	const float HealthPct = Health->GetHealthPercent();
	const float HP = Health->GetCurrentHealth();
	const float MaxHP = Health->GetMaxHealth();

	const float BarW = 260.f;
	const float HealthH = 20.f;
	const float StaminaH = 10.f;
	const float BarGap = 4.f;
	const float Margin = 28.f;
	const float HealthX = Margin;
	const float HealthY = Canvas->ClipY - HealthH - Margin;

	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), HealthX - 3.f, HealthY - 3.f, BarW + 6.f, HealthH + 6.f);
	DrawRect(FLinearColor(0.18f, 0.05f, 0.05f, 1.f), HealthX, HealthY, BarW, HealthH);
	DrawRect(FLinearColor(0.2f, 0.85f, 0.35f, 1.f), HealthX, HealthY, BarW * HealthPct, HealthH);

	if (Stamina)
	{
		const float StaminaPct = Stamina->GetStaminaPercent();
		const float StaminaX = HealthX;
		const float StaminaY = HealthY - StaminaH - BarGap;

		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), StaminaX - 3.f, StaminaY - 3.f, BarW + 6.f, StaminaH + 6.f);
		DrawRect(FLinearColor(0.06f, 0.08f, 0.18f, 1.f), StaminaX, StaminaY, BarW, StaminaH);
		DrawRect(FLinearColor(0.95f, 0.82f, 0.25f, 1.f), StaminaX, StaminaY, BarW * StaminaPct, StaminaH);
	}

	UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
	if (Font)
	{
		const FString HealthLine = Health->IsDead()
			? FString(TEXT("Health  —"))
			: FString::Printf(TEXT("Health  %.0f / %.0f"), HP, MaxHP);

		const float TextY = Stamina ? (HealthY - StaminaH - BarGap - 26.f) : (HealthY - 26.f);
		FCanvasTextItem HealthText(FVector2D(HealthX, TextY), FText::FromString(HealthLine), Font, FLinearColor::White);
		HealthText.Scale = FVector2D(1.15f, 1.15f);
		Canvas->DrawItem(HealthText);

		const FString Hint(TEXT("LMB: attack   |   LShift: dodge   |   E: interact"));
		FCanvasTextItem HintText(FVector2D(Margin, Canvas->ClipY - 96.f), FText::FromString(Hint), Font, FLinearColor(0.85f, 0.85f, 0.85f, 0.9f));
		HintText.Scale = FVector2D(1.f, 1.f);
		Canvas->DrawItem(HintText);

		const int32 Combo = Character->GetComboIndex();
		if (Character->IsAttacking() || Combo > 0)
		{
			const FString ComboStr = FString::Printf(TEXT("Combo x%d"), Combo + 1);
			FCanvasTextItem ComboText(FVector2D(HealthX + BarW + 16.f, HealthY - 2.f), FText::FromString(ComboStr), Font, FLinearColor(1.f, 0.9f, 0.3f, 1.f));
			ComboText.Scale = FVector2D(1.25f, 1.25f);
			Canvas->DrawItem(ComboText);
		}
	}
}

void AtestgameGameplayHUD::DrawEnemyHealthBars(APlayerController* PC)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector CameraLoc;
	FRotator CameraRot;
	PC->GetPlayerViewPoint(CameraLoc, CameraRot);
	const FVector CameraFwd = CameraRot.Vector();

	UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;

	for (TActorIterator<AEnemyCharacter> It(World); It; ++It)
	{
		AEnemyCharacter* Enemy = *It;
		if (!Enemy || Enemy->IsDead())
		{
			continue;
		}

		UHealthComponent* EnemyHealth = Enemy->GetHealthComponent();
		if (!EnemyHealth)
		{
			continue;
		}

		const FVector EnemyLoc = Enemy->GetActorLocation();
		const float DistSq = FVector::DistSquared(EnemyLoc, CameraLoc);
		if (DistSq > FMath::Square(EnemyBarMaxDistance))
		{
			continue;
		}

		const FVector ToEnemy = (EnemyLoc - CameraLoc).GetSafeNormal();
		if (FVector::DotProduct(CameraFwd, ToEnemy) <= 0.f)
		{
			continue;
		}

		float HalfHeight = 96.f;
		if (UCapsuleComponent* Capsule = Enemy->GetCapsuleComponent())
		{
			HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		}
		const FVector AnchorWorld = EnemyLoc + FVector(0.f, 0.f, HalfHeight + EnemyBarHeadroom);

		const FVector ScreenProj = Canvas->Project(AnchorWorld);
		if (ScreenProj.Z <= 0.f)
		{
			continue;
		}

		const float Dist = FMath::Sqrt(DistSq);
		const float Alpha = FMath::Clamp(1.f - (Dist / EnemyBarMaxDistance), 0.25f, 1.f);
		const float BarW = FMath::Lerp(40.f, 120.f, Alpha);
		const float BarH = FMath::Lerp(4.f, 8.f, Alpha);

		const float Pct = EnemyHealth->GetHealthPercent();
		const float X = ScreenProj.X - (BarW * 0.5f);
		const float Y = ScreenProj.Y - (BarH * 0.5f);

		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), X - 2.f, Y - 2.f, BarW + 4.f, BarH + 4.f);
		DrawRect(FLinearColor(0.22f, 0.05f, 0.05f, 1.f), X, Y, BarW, BarH);
		DrawRect(FLinearColor(0.95f, 0.2f, 0.2f, 1.f), X, Y, BarW * Pct, BarH);

		if (Font && Alpha > 0.5f)
		{
			const FString Label = FString::Printf(TEXT("%.0f / %.0f"),
				EnemyHealth->GetCurrentHealth(), EnemyHealth->GetMaxHealth());
			FCanvasTextItem LabelItem(FVector2D(X, Y - 16.f), FText::FromString(Label), Font, FLinearColor::White);
			LabelItem.Scale = FVector2D(0.9f, 0.9f);
			Canvas->DrawItem(LabelItem);
		}
	}
}
