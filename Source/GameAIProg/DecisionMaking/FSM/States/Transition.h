// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <functional>
/**
 * 
 */
namespace GameAI::FSM
{
	class State;
	struct Transition
	{
	public:
		State* From;
		State* To;
		std::function<bool()> Condition;
	};

	
}
