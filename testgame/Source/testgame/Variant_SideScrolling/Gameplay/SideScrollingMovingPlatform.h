
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SideScrollingInteractable.h"
#include "SideScrollingMovingPlatform.generated.h"

UCLASS(abstract)
class ASideScrollingMovingPlatform : public AActor, public ISideScrollingInteractable
{
	GENERATED_BODY()
	
public:
	
	ASideScrollingMovingPlatform();

protected:

	bool bMoving = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Moving Platform")
	FVector PlatformTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Moving Platform", meta = (ClampMin = 0, ClampMax = 10, Units="s"))
	float MoveDuration = 5.0f;

	UPROPERTY(EditAnywhere, Category="Moving Platform")
	bool bOneShot = false;

public:

	virtual void Interaction(AActor* Interactor) override;

	UFUNCTION(BlueprintCallable, Category="Moving Platform")
	virtual void ResetInteraction();

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Moving Platform", meta = (DisplayName="Move to Target"))
	void BP_MoveToTarget();

};
