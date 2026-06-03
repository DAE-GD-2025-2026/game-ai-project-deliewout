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
		AAIController* AIController = Cast<AAIController>(BB->GetOwner());
		if (AIController) AIController->MoveToLocation(PatrolPath[CurrentIndex]);
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
		if (Distance < 100.f)
		{
			CurrentIndex = (CurrentIndex + 1) % PatrolPath.Num();
			BB->SetValueAsVector(Target, PatrolPath[CurrentIndex]);
			AIController->MoveToLocation(PatrolPath[CurrentIndex]);
		}
	}
}

void Chase::OnUpdate(float DeltaTime, UBlackboardComponent* BB)
{
	AActor* TargetPlayer = Cast<AActor>(BB->GetValueAsObject(Agent));
	if (TargetPlayer)
	{
		FVector TargetLocation = TargetPlayer->GetActorLocation();
		BB->SetValueAsVector(Target, TargetLocation);
		AAIController* AIController = Cast<AAIController>(BB->GetOwner());
		if (AIController) AIController->MoveToLocation(TargetLocation);
	}
}

void Search::OnEnter(UBlackboardComponent* BB)
{
	SearchTimer = 5.0f;
	WanderCooldown = 2.0f;
	bReachedLastLocation = false;
	BB->SetValueAsBool("SearchExpired", false);

	AAIController* AIController = Cast<AAIController>(BB->GetOwner());
	if (AIController) AIController->MoveToLocation(BB->GetValueAsVector(Target));
}

void Search::OnUpdate(float DeltaTime, UBlackboardComponent* BB)
{
	SearchTimer -= DeltaTime;

	if (SearchTimer <= 0.0f)
	{
		BB->SetValueAsBool("SearchExpired", true);
		return;
	}

	AAIController* AIController = Cast<AAIController>(BB->GetOwner());
	if (!AIController || !AIController->GetPawn())return;

	FVector ActorLocation = AIController->GetPawn()->GetActorLocation();
	FVector SearchPos = BB->GetValueAsVector(Target);

	if (FVector::Dist(ActorLocation, SearchPos) < 100.f)
	{
		bReachedLastLocation = true;
	}
	if (bReachedLastLocation && SearchTimer > 0.0f)
	{
		WanderCooldown -= DeltaTime;
		if (WanderCooldown <= 0)
		{
			FVector randOffset = FVector(FMath::FRandRange(-500.f, 500.f), FMath::FRandRange(-500.f, 500.f), 0);
			FVector WanderTarget = SearchPos + randOffset;
			BB->SetValueAsVector(Target, WanderTarget);
			AIController->MoveToLocation(WanderTarget);
			WanderCooldown = 2.0f;
		}
	}
}
