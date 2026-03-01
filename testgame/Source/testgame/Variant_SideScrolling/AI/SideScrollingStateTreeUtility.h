
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "SideScrollingStateTreeUtility.generated.h"

class AAIController;

USTRUCT()
struct FStateTreeGetPlayerInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Context")
	TObjectPtr<APawn> NPC;

	UPROPERTY(VisibleAnywhere, Category="Context")
	TObjectPtr<AAIController> Controller;

	UPROPERTY(VisibleAnywhere, Category="Output")
	TObjectPtr<APawn> TargetPlayer;

	UPROPERTY(VisibleAnywhere, Category="Output")
	bool bValidTarget = false;

	UPROPERTY(EditAnywhere, Category="Parameter", meta = (ClampMin = 0, ClampMax = 10000, Units = "cm"))
	float RangeMax = 1000.0f;
};

USTRUCT(meta=(DisplayName="Get Player", Category="Side Scrolling"))
struct FStateTreeGetPlayerTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeGetPlayerInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};