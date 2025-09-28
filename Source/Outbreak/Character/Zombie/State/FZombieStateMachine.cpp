#include "FZombieStateMachine.h"

void FZombieStateMachine::ChangeState(const EZombieStateType Key, const TObjectPtr<ACharacterPlayer> Context)
{
	if (IsInState(EZombieStateType::Die))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Zombie is dead, cannot change state."), CURRENT_CONTEXT);
		return;
	}
	
	TStateMachine::ChangeState(Key, Context);
}
