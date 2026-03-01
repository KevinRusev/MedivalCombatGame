
#include "SideScrollingAIController.h"
#include "GameplayStateTreeModule/Public/Components/StateTreeAIComponent.h"

ASideScrollingAIController::ASideScrollingAIController()
{
	
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAI"));
	check(StateTreeAI);

	bStartAILogicOnPossess = true;

	bAttachToPawn = true;
}
