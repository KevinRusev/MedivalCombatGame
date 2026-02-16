
#include "CombatStateTreeUtility.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "CombatEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "StateTreeAsyncExecutionContext.h"

bool FStateTreeCharacterGroundedCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	bool bCondition = InstanceData.Character->GetMovementComponent()->IsMovingOnGround();

	return InstanceData.bMustBeOnAir ? !bCondition : bCondition;
}

#if WITH_EDITOR
FText FStateTreeCharacterGroundedCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Is Character Grounded</b>");
}
#endif

EStateTreeRunStatus FStateTreeComboAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Character->OnAttackCompleted.BindLambda(
			[WeakContext = Context.MakeWeakExecutionContext()]()
			{
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
			}
		);

		InstanceData.Character->DoAIComboAttack();
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeComboAttackTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Character->OnAttackCompleted.Unbind();
	}
}

#if WITH_EDITOR
FText FStateTreeComboAttackTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Do Combo Attack</b>");
}
#endif

EStateTreeRunStatus FStateTreeChargedAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Character->OnAttackCompleted.BindLambda(
			[WeakContext = Context.MakeWeakExecutionContext()]()
			{
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
			}
		);

		InstanceData.Character->DoAIChargedAttack();
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeChargedAttackTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Character->OnAttackCompleted.Unbind();
	}
}

#if WITH_EDITOR
FText FStateTreeChargedAttackTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Do Charged Attack</b>");
}
#endif

EStateTreeRunStatus FStateTreeWaitForLandingTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Character->OnEnemyLanded.BindLambda(
			[WeakContext = Context.MakeWeakExecutionContext()]()
			{
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
			}
		);
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeWaitForLandingTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Character->OnEnemyLanded.Unbind();
	}
}

#if WITH_EDITOR
FText FStateTreeWaitForLandingTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Wait for Landing</b>");
}
#endif

EStateTreeRunStatus FStateTreeFaceActorTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Controller->SetFocus(InstanceData.ActorToFaceTowards);
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeFaceActorTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

#if WITH_EDITOR
FText FStateTreeFaceActorTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Face Towards Actor</b>");
}
#endif

EStateTreeRunStatus FStateTreeFaceLocationTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Controller->SetFocalPoint(InstanceData.FaceLocation);
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeFaceLocationTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

#if WITH_EDITOR
FText FStateTreeFaceLocationTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Face Towards Location</b>");
}
#endif

EStateTreeRunStatus FStateTreeSetCharacterSpeedTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	
	if (Transition.ChangeType == EStateTreeStateChangeType::Changed)
	{
		
		FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

		InstanceData.Character->GetCharacterMovement()->MaxWalkSpeed = InstanceData.Speed;
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FStateTreeSetCharacterSpeedTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Set Character Speed</b>");
}
#endif

EStateTreeRunStatus FStateTreeGetPlayerInfoTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.TargetPlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(InstanceData.Character, 0));

	if (InstanceData.TargetPlayerCharacter)
	{
		
		InstanceData.TargetPlayerLocation = InstanceData.TargetPlayerCharacter->GetActorLocation();
	}

	InstanceData.DistanceToTarget = FVector::Distance(InstanceData.TargetPlayerLocation, InstanceData.Character->GetActorLocation());

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FStateTreeGetPlayerInfoTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting ) const
{
	return FText::FromString("<b>Get Player Info</b>");
}
#endif