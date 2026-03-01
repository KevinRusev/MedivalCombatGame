
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SideScrollingInteractable.h"
#include "SideScrollingNPC.generated.h"

UCLASS(abstract)
class ASideScrollingNPC : public ACharacter, public ISideScrollingInteractable
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category="NPC", meta = (ClampMin = 0, ClampMax = 10000, Units="cm/s"))
	float LaunchImpulse = 500.0f;

	UPROPERTY(EditAnywhere, Category="NPC", meta = (ClampMin = 0, ClampMax = 10000, Units="cm/s"))
	float LaunchVerticalImpulse = 500.0f;

	UPROPERTY(EditAnywhere, Category="NPC", meta = (ClampMin = 0, ClampMax = 10, Units="s"))
	float DeactivationTime = 3.0f;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="NPC")
	bool bDeactivated = false;

	FTimerHandle DeactivationTimer;

public:

	ASideScrollingNPC();

public:

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:

	virtual void Interaction(AActor* Interactor) override;

	void ResetDeactivation();
};
