
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SideScrollingSoftPlatform.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS(abstract)
class ASideScrollingSoftPlatform : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionCheckBox;

public:
	
	ASideScrollingSoftPlatform();

protected:

	UFUNCTION()
	void OnSoftCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};
