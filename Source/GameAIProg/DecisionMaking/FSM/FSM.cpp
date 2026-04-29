// Fill out your copyright notice in the Description page of Project Settings.


#include "DecisionMaking/FSM/FSM.h"
#include "States/Transition.h"

using namespace GameAI::FSM;

void FSM::Update(float DeltaTime, UBlackboardComponent* Blackboard)
{
	if (!CurrentState) return;

	for (auto& Transition : Transitions[CurrentState])
	{
		if (Transition.Condition)
		{

		}
	}
}

void FSM::ChangeState(State* NewState)
{
}
