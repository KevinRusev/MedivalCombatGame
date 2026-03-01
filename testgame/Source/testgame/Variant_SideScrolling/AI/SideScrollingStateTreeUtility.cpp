
#include "SideScrollingStateTreeUtility.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

EStateTreeRunStatus FStateTreeGetPlayerTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.TargetPlayer = UGameplayStatics::GetPlayerPawn(InstanceData.Controller.Get(), 0);

	if (IsValid(InstanceData.TargetPlayer) && IsValid(InstanceData.NPC))
	{
		InstanceData.bValidTarget = FVector::Distance(InstanceData.NPC->GetActorLocation(), InstanceData.TargetPlayer->GetActorLocation()) < InstanceData.RangeMax;
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FStateTreeGetPlayerTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Get Player</b>");
}
#endif