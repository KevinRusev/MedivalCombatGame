
#include "InventoryComponent.h"
#include "testgame.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxCapacity = 20;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Items.SetNum(MaxCapacity);
}

bool UInventoryComponent::AddItem(const FItemData& Item)
{
	if (!Item.IsValid())
	{
		UE_LOG(Logtestgame, Warning, TEXT("Attempted to add invalid item to inventory"));
		return false;
	}

	if (Item.MaxStackSize > 1)
	{
		for (int32 i = 0; i < Items.Num(); ++i)
		{
			if (Items[i].ItemID == Item.ItemID && Items[i].Quantity < Items[i].MaxStackSize)
			{
				int32 SpaceAvailable = Items[i].MaxStackSize - Items[i].Quantity;
				int32 AmountToAdd = FMath::Min(SpaceAvailable, Item.Quantity);
				Items[i].Quantity += AmountToAdd;
				
				OnItemAdded.Broadcast(Items[i], i);
				BroadcastUpdate();
				
				if (AmountToAdd < Item.Quantity)
				{
					FItemData RemainingItem = Item;
					RemainingItem.Quantity = Item.Quantity - AmountToAdd;
					return AddItem(RemainingItem);
				}
				
				return true;
			}
		}
	}

	int32 EmptySlot = FindEmptySlot();
	if (EmptySlot == -1)
	{
		UE_LOG(Logtestgame, Warning, TEXT("Inventory is full, cannot add item: %s"), *Item.ItemID.ToString());
		return false;
	}

	Items[EmptySlot] = Item;
	
	OnItemAdded.Broadcast(Items[EmptySlot], EmptySlot);
	BroadcastUpdate();
	
	return true;
}

bool UInventoryComponent::RemoveItem(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= Items.Num())
	{
		return false;
	}

	if (!Items[SlotIndex].IsValid())
	{
		return false;
	}

	FItemData RemovedItem = Items[SlotIndex];
	Items[SlotIndex] = FItemData();
	
	OnItemRemoved.Broadcast(RemovedItem, SlotIndex);
	BroadcastUpdate();
	
	return true;
}

int32 UInventoryComponent::RemoveItemByID(FName ItemID, int32 Quantity)
{
	int32 TotalRemoved = 0;

	for (int32 i = 0; i < Items.Num() && TotalRemoved < Quantity; ++i)
	{
		if (Items[i].ItemID == ItemID)
		{
			int32 AmountToRemove = FMath::Min(Items[i].Quantity, Quantity - TotalRemoved);
			Items[i].Quantity -= AmountToRemove;
			TotalRemoved += AmountToRemove;

			if (Items[i].Quantity <= 0)
			{
				FItemData RemovedItem = Items[i];
				Items[i] = FItemData();
				OnItemRemoved.Broadcast(RemovedItem, i);
			}
		}
	}

	if (TotalRemoved > 0)
	{
		BroadcastUpdate();
	}

	return TotalRemoved;
}

FItemData UInventoryComponent::GetItemAtSlot(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= Items.Num())
	{
		return FItemData();
	}

	return Items[SlotIndex];
}

bool UInventoryComponent::HasSpaceForItem(const FItemData& Item) const
{
	if (!Item.IsValid())
	{
		return false;
	}

	if (Item.MaxStackSize > 1)
	{
		for (const FItemData& ExistingItem : Items)
		{
			if (ExistingItem.ItemID == Item.ItemID && ExistingItem.Quantity < ExistingItem.MaxStackSize)
			{
				return true;
			}
		}
	}

	return FindEmptySlot() != -1;
}

int32 UInventoryComponent::GetItemCount(FName ItemID) const
{
	int32 Count = 0;
	for (const FItemData& Item : Items)
	{
		if (Item.ItemID == ItemID)
		{
			Count += Item.Quantity;
		}
	}
	return Count;
}

bool UInventoryComponent::HasItem(FName ItemID, int32 Quantity) const
{
	return GetItemCount(ItemID) >= Quantity;
}

int32 UInventoryComponent::GetCurrentItemCount() const
{
	int32 Count = 0;
	for (const FItemData& Item : Items)
	{
		if (Item.IsValid())
		{
			++Count;
		}
	}
	return Count;
}

bool UInventoryComponent::UseItem(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= Items.Num())
	{
		return false;
	}

	FItemData& Item = Items[SlotIndex];
	if (!Item.IsValid() || !Item.bCanUse)
	{
		return false;
	}

	Item.Quantity--;

	if (Item.Quantity <= 0)
	{
		FItemData UsedItem = Item;
		Items[SlotIndex] = FItemData();
		OnItemRemoved.Broadcast(UsedItem, SlotIndex);
	}

	BroadcastUpdate();
	return true;
}

int32 UInventoryComponent::FindSlotForItem(const FItemData& Item) const
{
	
	if (Item.MaxStackSize > 1)
	{
		for (int32 i = 0; i < Items.Num(); ++i)
		{
			if (Items[i].ItemID == Item.ItemID && Items[i].Quantity < Items[i].MaxStackSize)
			{
				return i;
			}
		}
	}

	return FindEmptySlot();
}

int32 UInventoryComponent::FindEmptySlot() const
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (!Items[i].IsValid())
		{
			return i;
		}
	}
	return -1;
}

void UInventoryComponent::BroadcastUpdate()
{
	OnInventoryUpdated.Broadcast(Items);
}
