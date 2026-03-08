
#include "AnimNotify_EndDash.h"
#include "PlatformingCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_EndDash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	
	if (APlatformingCharacter* PlatformingCharacter = Cast<APlatformingCharacter>(MeshComp->GetOwner()))
	{
		
		PlatformingCharacter->EndDash();
	}
}

FString UAnimNotify_EndDash::GetNotifyName_Implementation() const
{
	return FString("End Dash");
}
