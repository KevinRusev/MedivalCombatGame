
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SideScrollingPickup.generated.h"

class USphereComponent;

UCLASS(abstract)
class ASideScrollingPickup : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* Sphere;

public:

	ASideScrollingPickup();

protected:

	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent, Category="Pickup", meta = (DisplayName = "On Picked Up"))
	void BP_OnPickedUp();
};
