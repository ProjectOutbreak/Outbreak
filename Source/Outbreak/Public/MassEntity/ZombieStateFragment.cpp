#include "ZombieStateFragment.h"
#include "Character/Zombie/State/FZombieIdleState.h"

FZombieStateFragment::FZombieStateFragment()
	: CurrentState(EZombieStateType::Idle),
	  bIsAlert(false)
{
}
