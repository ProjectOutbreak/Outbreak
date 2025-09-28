// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

template <typename T, typename ContextType>
class TStateMachine;

template <typename T, typename ContextType = UObject>
class TState
{
public:
	virtual ~TState() = default;

	TState(TSharedPtr<TStateMachine<T, ContextType>> InFsm, T InStateKey)
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
	 * @param Context Optional context object that can be passed to the state.
	 */
	virtual void Enter(T PreviousState, TObjectPtr<ContextType> Context = nullptr) = 0;

	/*
	 * Called every frame while the state is active.
	 * @param CurrentState The current state.
	 * @param DeltaTime The time since the last frame.
	 */
	virtual void Execute(T CurrentState, float DeltaTime) = 0;

	/*
	 * Called when the state is exited.
	 * @param NextState The state that will be active next.
	 * @param Context Optional context object that can be passed to the state.
	 */
	virtual void Exit(T NextState, TObjectPtr<ContextType> Context = nullptr) = 0;

	/*
	 * Return the name of the state.
	 */
	virtual FString GetName() const 
	{
		return *UEnum::GetValueAsString(StateKey);
	}

protected:
	TSharedPtr<TStateMachine<T, ContextType>> Fsm;
	T StateKey;
	virtual void ChangeState(T Key, TObjectPtr<ContextType> Context = nullptr)
	{
		Fsm->ChangeState(Key, Context);
	}
};
