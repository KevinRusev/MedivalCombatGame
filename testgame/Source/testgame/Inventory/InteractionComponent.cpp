
#include "InteractionComponent.h"
#include "PickupItem.h"
#include "InventoryComponent.h"
#include "testgameCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "testgame.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	InteractionDistance = 500.f;
	TraceChannel = ECC_Visibility;
	CurrentLookAtItem = nullptr;
	PreviousLookAtItem = nullptr;
	InventoryComponent = nullptr;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PerformInteractionTrace();

	if (CurrentLookAtItem != PreviousLookAtItem)
	{
		
		if (PreviousLookAtItem && IsValid(PreviousLookAtItem))
		{
			PreviousLookAtItem->SetHighlighted(false);
			OnStopLookingAtItem.Broadcast();
		}

		if (CurrentLookAtItem)
		{
			CurrentLookAtItem->SetHighlighted(true);
			OnLookAtItem.Broadcast(CurrentLookAtItem);
		}

		PreviousLookAtItem = CurrentLookAtItem;
	}
}

bool UInteractionComponent::TryInteract()
{
	if (!CurrentLookAtItem || !CurrentLookAtItem->CanBePickedUp())
	{
		UE_LOG(Logtestgame, Log, TEXT("TryInteract: No item to pick up or item cannot be picked up"));
		return false;
	}

	if (!InventoryComponent)
	{
		UE_LOG(Logtestgame, Warning, TEXT("InteractionComponent has no InventoryComponent set"));
		return false;
	}

	FItemData ItemData = CurrentLookAtItem->GetItemData();
	if (!InventoryComponent->HasSpaceForItem(ItemData))
	{
		UE_LOG(Logtestgame, Log, TEXT("Inventory is full, cannot pick up item: %s"), *ItemData.ItemID.ToString());
		return false;
	}

	if (!ItemData.HeldMesh && CurrentLookAtItem->GetItemMesh() && CurrentLookAtItem->GetItemMesh()->GetStaticMesh())
	{
		ItemData.HeldMesh = CurrentLookAtItem->GetItemMesh()->GetStaticMesh();
	}

	if (InventoryComponent->AddItem(ItemData))
	{
		UE_LOG(Logtestgame, Log, TEXT("Successfully picked up item: %s"), *ItemData.ItemID.ToString());
		
		AtestgameCharacter* Character = Cast<AtestgameCharacter>(GetOwner());
		if (Character)
		{
			Character->EquipItem(ItemData);
		}
		
		CurrentLookAtItem->OnPickedUp(GetOwner());
		
		CurrentLookAtItem = nullptr;
		PreviousLookAtItem = nullptr;
		
		return true;
	}

	return false;
}

void UInteractionComponent::PerformInteractionTrace()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * InteractionDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = false;

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		TraceChannel,
		FCollisionShape::MakeSphere(30.f),
		QueryParams
	);

	if (bHit && HitResult.GetActor())
	{
		APickupItem* HitItem = Cast<APickupItem>(HitResult.GetActor());
		if (HitItem && HitItem->CanBePickedUp())
		{
			CurrentLookAtItem = HitItem;
			return;
		}
	}

	CurrentLookAtItem = nullptr;
}
