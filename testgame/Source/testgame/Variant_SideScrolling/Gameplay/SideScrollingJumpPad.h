
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SideScrollingJumpPad.generated.h"

class UBoxComponent;

UCLASS(abstract)
class ASideScrollingJumpPad : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Box;

protected:

	UPROPERTY(EditAnywhere, Category="Jump Pad", meta = (ClampMin=0, ClampMax=10000, Units="cm/s"))
	float ZStrength = 1000.0f;

public:

	ASideScrollingJumpPad();

protected:

	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

};
