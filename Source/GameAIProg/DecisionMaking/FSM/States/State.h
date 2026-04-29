// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AIController.h"
#include <functional>
/**
 * 
 */
namespace GameAI::FSM
{
	class State
	{
	public:
		virtual ~State()=default;
		virtual void OnEnter(UBlackboardComponent* BB){}
		virtual void OnUpdate(float deltaTime, UBlackboardComponent* BB) = 0;
		virtual void OnExit(UBlackboardComponent* BB) {}

	};

	struct Transition
	{
	public:
		State* From;
		State* To;
		std::function<bool()> Condition;
	};

	class Patrol : public State
	{
	public:
		Patrol(const TArray<FVector>& Path) : PatrolPath(Path) {}
		virtual void OnEnter(UBlackboardComponent* BB) override;
		virtual void OnUpdate(float DeltaTime, UBlackboardComponent* BB) override;
	private:
		TArray<FVector> PatrolPath;
		int32 CurrentIndex = 0;
	};

	class Search : public State
	{
	public:
		virtual void OnEnter(UBlackboardComponent* BB) override;
		virtual void OnUpdate(float DeltaTime, UBlackboardComponent* BB) override;
	private:
		float SearchTimer=0.0f;
		float WanderCooldown = 0.0f;
		bool bReachedLastLocation = false;
	};

	class Chase : public State
	{
	public:
		virtual void OnUpdate(float DeltaTime, UBlackboardComponent* BB) override;
	};
}
