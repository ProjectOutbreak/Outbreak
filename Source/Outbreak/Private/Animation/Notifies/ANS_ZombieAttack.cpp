#include "Animation/Notifies/ANS_ZombieAttack.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"

void UANS_ZombieAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (ACharacterZombie* Zombie = Cast<ACharacterZombie>(MeshComp->GetOwner()))
	{
		Zombie->EnableAttackCollision();
	}
}

void UANS_ZombieAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (ACharacterZombie* Zombie = Cast<ACharacterZombie>(MeshComp->GetOwner()))
	{
		Zombie->DisableAttackCollision();
	}
}