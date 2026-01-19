// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * Data structure representing an item in the inventory
 */
USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	/** Unique identifier for the item type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	/** Display name shown in UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	/** Description shown in UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText Description;

	/** Icon to display in inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* Icon;

	/** How many of this item can stack in one slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize;

	/** Current quantity (for stacks) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity;

	/** Whether this item can be used/consumed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanUse;

	/** Mesh to use when item is held in hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMesh* HeldMesh;

	FItemData()
		: ItemID(NAME_None)
		, DisplayName(FText::GetEmpty())
		, Description(FText::GetEmpty())
		, Icon(nullptr)
		, MaxStackSize(1)
		, Quantity(1)
		, bCanUse(false)
		, HeldMesh(nullptr)
	{
	}

	bool IsValid() const
	{
		return !ItemID.IsNone();
	}
};

/**
 * Base class for items that can be picked up from the world
 */
UCLASS(Blueprintable)
class TESTGAME_API APickupItem : public AActor
{
	GENERATED_BODY()

public:
	APickupItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/** Called when a player picks up this item */
	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void OnPickedUp(AActor* PickingActor);

	/** Get the item data for this pickup */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FItemData GetItemData() const { return ItemData; }

	/** Check if this item can be picked up */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool CanBePickedUp() const { return bCanBePickedUp; }

	/** Enable or disable highlighting when player looks at item */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetHighlighted(bool bHighlight);

	/** Get the static mesh component */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }

protected:
	/** Collision sphere for interaction detection */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionSphere;

	/** Visual mesh representation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ItemMesh;

	/** The data for this item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FItemData ItemData;

	/** Whether this item can currently be picked up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bCanBePickedUp;

	/** Rotation speed for the floating effect (degrees per second) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float RotationSpeed;

	/** Bobbing amplitude */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float BobAmplitude;

	/** Bobbing speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float BobSpeed;

	/** Highlight color when player looks at item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FLinearColor HighlightColor;

private:
	/** Starting location for bobbing effect */
	FVector StartLocation;

	/** Time accumulator for bobbing */
	float BobTime;

	/** Original materials for restoring after highlight */
	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	/** Dynamic material instances for highlighting */
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	/** Whether currently highlighted */
	bool bIsHighlighted;
};
