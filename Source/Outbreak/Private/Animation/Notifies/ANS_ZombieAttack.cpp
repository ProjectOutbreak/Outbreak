#include "Animation/Notifies/ANS_ZombieAttack.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

void UANS_ZombieAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (IZombieInterface* Zombie = Cast<IZombieInterface>(MeshComp->GetOwner()))
	{
		Zombie->SetEnableAttackCollision(true);
	}
}

void UANS_ZombieAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (IZombieInterface* Zombie = Cast<IZombieInterface>(MeshComp->GetOwner()))
	{
		Zombie->SetEnableAttackCollision(false);
	}
}