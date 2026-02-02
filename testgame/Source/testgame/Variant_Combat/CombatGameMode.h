
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CombatGameMode.generated.h"

UCLASS(abstract)
class ACombatGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACombatGameMode();
};
