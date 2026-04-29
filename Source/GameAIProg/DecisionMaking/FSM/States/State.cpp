// Fill out your copyright notice in the Description page of Project Settings.


#include "DecisionMaking/FSM/States/State.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

FName Target = "MoveTarget";
FName Agent = "TargetPlayer";

using namespace GameAI::FSM;

void Patrol::OnEnter(UBlackboardComponent* BB)
{
	if (PatrolPath.Num() > 0)
	{
		BB->SetValueAsVector(Target, PatrolPath[CurrentIndex]);
	}
}

void Patrol::OnUpdate(float DeltaTime, UBlackboardComponent* BB)
{
	if (PatrolPath.Num() == 0) return;
	AAIController* AIController = Cast<AAIController>(BB->GetOwner());
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (ControlledPawn)
	{
		float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PatrolPath[CurrentIndex]);
		CurrentIndex = (CurrentIndex + 1) % PatrolPath.Num();

		BB->SetValueAsVector(Target, PatrolPath[CurrentIndex]);
	}
}

void Chase::OnUpdate(float DeltaTime, UBlackboardComponent* BB)
{
	AActor* TargetPlayer = Cast<AActor>(BB->GetValueAsObject(Agent));
	if (TargetPlayer)
	{
		BB->SetValueAsVector(Target, TargetPlayer->GetActorLocation());
	}
}

void Search::OnEnter(UBlackboardComponent* BB)
{
}

void Search::OnUpdate(float DeltaTime, UBlackboardComponent* BB)
{
}
