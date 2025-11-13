#include "FZombieStateMachine.h"
#include "Utilities/DebugHelper.h"

void FZombieStateMachine::ChangeState(const EZombieStateType Key)
{
	if (IsInState(EZombieStateType::Die))
	{
		const FString DebugMsg = FString::Printf(TEXT("Zombie is dead, cannot change state."));
		PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
		return;
	}
	
	TStateMachine::ChangeState(Key);
}
