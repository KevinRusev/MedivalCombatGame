
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMesh* HeldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AActor> HeldActorClass;

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

	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void OnPickedUp(AActor* PickingActor);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	FItemData GetItemData() const { return ItemData; }

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool CanBePickedUp() const { return bCanBePickedUp; }

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetHighlighted(bool bHighlight);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FItemData ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bCanBePickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float BobAmplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float BobSpeed;

private:
	
	FVector StartLocation;

	float BobTime;

	bool bIsHighlighted;
};
