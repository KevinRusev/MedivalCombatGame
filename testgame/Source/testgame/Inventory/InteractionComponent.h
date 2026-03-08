
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class APickupItem;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLookAtItem, APickupItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopLookingAtItem);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESTGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool TryInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	APickupItem* GetCurrentLookAtItem() const { return CurrentLookAtItem; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInventoryComponent(UInventoryComponent* InInventory) { InventoryComponent = InInventory; }

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnLookAtItem OnLookAtItem;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnStopLookingAtItem OnStopLookingAtItem;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

private:
	
	void PerformInteractionTrace();

	UPROPERTY()
	APickupItem* CurrentLookAtItem;

	UPROPERTY()
	APickupItem* PreviousLookAtItem;
};
