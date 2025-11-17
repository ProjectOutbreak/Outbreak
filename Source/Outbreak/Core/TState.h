// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

template <typename T>
class TStateMachine;

template <typename T>
class TState
{
public:
	virtual ~TState() = default;

	TState(TSharedPtr<TStateMachine<T>> InFsm, T InStateKey)
	{
		if (!InFsm)
		{
			UE_LOG(LogTemp, Error, TEXT("StateMachine is null"));
		}
		Fsm = InFsm;
		StateKey = InStateKey;
	};

	/*
	 * Called when the state is entered.
	 * @param PreviousState The state that was active before this one.
	 */
	virtual void Enter(T PreviousState) = 0;

	/*
	 * Called every frame while the state is active.
	 * @param CurrentState The current state.
	 * @param DeltaTime The time since the last frame.
	 */
	virtual void Execute(T CurrentState, float DeltaTime) = 0;

	/*
	 * Called when the state is exited.
	 * @param NextState The state that will be active next.
	 */
	virtual void Exit(T NextState) = 0;

	/*
	 * Return the name of the state.
	 */
	virtual FString GetName() const 
	{
		return *UEnum::GetValueAsString(StateKey);
	}

protected:
	TSharedPtr<TStateMachine<T>> Fsm;
	T StateKey;
	virtual void ChangeState(T Key)
	{
		Fsm->ChangeState(Key);
	}
};
