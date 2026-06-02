// Fill out your copyright notice in the Description page of Project Settings.


#include "Level_FSM.h"

#include "FSMComponent.h"
#include "DecisionMaking/GameAIController.h"
#include "States/State.h"
#include <functional>
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
ALevel_FSM::ALevel_FSM()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_FSM::BeginPlay()
{
	Super::BeginPlay();
	
	Agent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, 
	FVector{0,0,90}, FRotator::ZeroRotator);
	Agent->SetDebugRenderingEnabled(false);

	TArray<FVector> PatrolPoints = { FVector(0,0,90), FVector(1000,0,90),FVector(1000,1000,90),FVector(0,1000,90) };
	
	if (AGameAIController* AIController = Cast<AGameAIController>(Agent->GetController()))
	{
		if (UFSMComponent* FSM = Cast<UFSMComponent>(AIController->GetBrainComponent()))
		{
			auto PatrolState = std::make_unique<GameAI::FSM::Patrol>(PatrolPoints);
			auto SearchState = std::make_unique<GameAI::FSM::Search>();
			auto ChaseState = std::make_unique<GameAI::FSM::Chase>();

			GameAI::FSM::State* PatrolPtr = PatrolState.get();
			GameAI::FSM::State* SearchPtr = SearchState.get();
			GameAI::FSM::State* ChasePtr = ChaseState.get();

			FSM->AddState(std::move(PatrolState),true);
			FSM->AddState(std::move(SearchState));
			FSM->AddState(std::move(ChaseState));

			UBlackboardComponent* BB = AIController->GetBlackboardComponent();

			auto IsTargetVisible = [AIController, BB]()->bool
				{
					AActor* TargetPlayer = Cast<AActor>(BB->GetValueAsObject("TargetPlayer"));
					if (!TargetPlayer || !AIController->GetPawn()) return false;
					const float Dist = FVector::Dist(AIController->GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());
					return Dist < 1500.f && AIController->LineOfSightTo(TargetPlayer);
				};
			

			auto IsTargetNotVisible = [IsTargetVisible]()->bool
				{
					return !IsTargetVisible();
				};

			auto IsSearchingTooLong = [BB]()->bool
				{
					return BB->GetValueAsBool("SearchExpired");
				};

			FSM->AddTransition(PatrolPtr, ChasePtr, IsTargetVisible);
			FSM->AddTransition(ChasePtr, SearchPtr, IsTargetNotVisible);
			FSM->AddTransition(SearchPtr, ChasePtr, IsTargetVisible);
			FSM->AddTransition(SearchPtr, PatrolPtr, IsSearchingTooLong);

			AIController->RunFiniteStateMachine();
		}
	}
	
}

// Called every frame
void ALevel_FSM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

