// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include <memory>
#include <vector>
#include <map>
//#include "States/Transition.h"
#include "States/State.h"

/**
 * 
 */
namespace GameAI::FSM 
{
	
	class FSM
	{
	public:
		void SetInitialState(State* InitialState) { CurrentState = InitialState; }
		void AddState(std::unique_ptr<State> NewState) { States.push_back(std::move(NewState)); }
		void AddTransition(State* From, Transition T) { Transitions[From].push_back(T); }
		void Update(float DeltaTime, UBlackboardComponent* Blackboard);
		void ChangeState(State* NewState, UBlackboardComponent* Blackboard);
		

	private:
		State* CurrentState = nullptr;
		std::vector<std::unique_ptr<State>> States;
		std::map<State*, std::vector<Transition>> Transitions;
	};
}

