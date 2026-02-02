
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "testgameGameplayHUD.generated.h"

class AtestgameCharacter;

UCLASS()
class TESTGAME_API AtestgameGameplayHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	
	void DrawPlayerHealth(AtestgameCharacter* Character);

	void DrawEnemyHealthBars(APlayerController* PC);
};
