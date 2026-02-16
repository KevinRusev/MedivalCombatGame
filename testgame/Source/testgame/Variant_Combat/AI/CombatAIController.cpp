
#include "CombatAIController.h"
#include "Components/StateTreeAIComponent.h"

ACombatAIController::ACombatAIController()
{
	
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAI"));
	check(StateTreeAI);

	bStartAILogicOnPossess = true;

	bAttachToPawn = true;
}
