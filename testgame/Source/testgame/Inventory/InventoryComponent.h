
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupItem.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, const TArray<FItemData>&, Items);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, const FItemData&, Item, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, const FItemData&, Item, int32, SlotIndex);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESTGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FItemData& Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItemByID(FName ItemID, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemData GetItemAtSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FItemData> GetAllItems() const { return Items; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasSpaceForItem(const FItemData& Item) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCount(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName ItemID, int32 Quantity = 1) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetCurrentItemCount() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetMaxCapacity() const { return MaxCapacity; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(int32 SlotIndex);

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemRemoved OnItemRemoved;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxCapacity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemData> Items;

private:
	
	int32 FindSlotForItem(const FItemData& Item) const;

	int32 FindEmptySlot() const;

	void BroadcastUpdate();
};
