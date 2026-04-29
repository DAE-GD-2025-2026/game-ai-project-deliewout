// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include <memory>
#include <vector>
#include <map>

/**
 * 
 */
namespace GameAI::FSM 
{
	class State;
	struct Transition;
	class FSM
	{
	public:

		void Update(float DeltaTime, UBlackboardComponent* Blackboard);
		void ChangeState(State* NewState);

	private:
		State* CurrentState = nullptr;
		std::vector<std::unique_ptr<State>> States;
		std::map<State*, std::vector<Transition>> Transitions;
	};
}

