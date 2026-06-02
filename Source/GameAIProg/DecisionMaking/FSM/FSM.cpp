// Fill out your copyright notice in the Description page of Project Settings.


#include "DecisionMaking/FSM/FSM.h"
#include "States/Transition.h"
#include "States/State.h"

using namespace GameAI::FSM;

void FSM::Update(float DeltaTime, UBlackboardComponent* Blackboard)
{
	if (!CurrentState) return;

	for (auto& Transition : Transitions[CurrentState])
	{
		if (Transition.Condition && Transition.Condition())
		{
			ChangeState(Transition.To,Blackboard);
			break;
		}
	}
	CurrentState->OnUpdate(DeltaTime, Blackboard);
}

void FSM::ChangeState(State* NewState, UBlackboardComponent* Blackboard)
{
	if (CurrentState) CurrentState->OnExit(Blackboard);
	CurrentState = NewState;
	CurrentState->OnEnter(Blackboard);
}
