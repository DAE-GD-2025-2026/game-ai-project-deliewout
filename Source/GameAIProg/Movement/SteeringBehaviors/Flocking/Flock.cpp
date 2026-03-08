#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"


Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
	Agents.SetNum(FlockSize);

	pSeparationBehavior = std::make_unique<Separation>(this);
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pVelMatchBehavior = std::make_unique<VelocityMatch>(this);
	pSeekBehavior = std::make_unique<Seek>();
	pWanderBehavior = std::make_unique<Wander>();


	pBlendedSteering = std::make_unique<BlendedSteering>(
		std::vector<BlendedSteering::WeightedBehavior>{
			{pSeparationBehavior.get(), 0.2f},
			{ pCohesionBehavior.get(), 0.6f },
			{ pVelMatchBehavior.get(), 0.2f },
			{ pSeekBehavior.get(),0.2f },
			{ pWanderBehavior.get(),0.8f }

	}
	);

	pCurrentSteeringBehaviour = pBlendedSteering.get();
	if (pAgentToEvade)
	{
		pEvadeBehavior = std::make_unique<Evade>();
		pEvadeBehavior->SetTarget(FSteeringParams{ pAgentToEvade->GetPosition() });
		pPrioritySteering = std::make_unique<PrioritySteering>(std::vector<ISteeringBehavior*>
		{
			pEvadeBehavior.get(),
			pBlendedSteering.get()
		}
		);
		pCurrentSteeringBehaviour = pPrioritySteering.get();
	}
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	OldPositions.SetNum(FlockSize);
	pPartitionedSpace = std::make_unique<CellSpace>(pWorld, WorldSize*2.f, WorldSize*2.f, NrOfCellsX, NrOfCellsX, FlockSize);
#else
	Neighbors.SetNum(FlockSize);
#endif
	//initialize the flock and the memory pool
	for (int i = 0; i < FlockSize; ++i)
	{
		if (ASteeringAgent* agent = pWorld->SpawnActor<ASteeringAgent>(AgentClass, FVector{ FMath::FRandRange(-WorldSize / 2, WorldSize / 2),FMath::FRandRange(-WorldSize / 2, WorldSize / 2),90.0f }, FRotator::ZeroRotator))
		{
			agent->SetActorTickEnabled(false);
			agent->SetSteeringBehavior(pCurrentSteeringBehaviour);
			agent->SetDebugRenderingEnabled(false);
			Agents[i] = agent;
		}
		else
		{
			--i;
		}
	}

}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
#ifndef GAMEAI_USE_SPACE_PARTITIONING
	Neighbors.Empty();
#endif
	Agents.Empty();
}

void Flock::Tick(float DeltaTime)
{
 // TODO: update the flock
 // TODO: for every agent: done
  // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent) done
  // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool) done
  // TODO: trim the agent to the world
	if (pAgentToEvade)
	{
		pEvadeBehavior->SetTarget(FSteeringParams{ pAgentToEvade->GetPosition() });
	}
	for (int i = 0; i < Agents.Num(); ++i)
	{
		ASteeringAgent* Agent = Agents[i];
#ifdef GAMEAI_USE_SPACE_PARTITIONING
		pPartitionedSpace->RegisterNeighbors(*Agent, NeighborhoodRadius);
		pPartitionedSpace->UpdateAgentCell(*Agent, OldPositions[i]);
		OldPositions[i] = Agent->GetPosition();
#else
		RegisterNeighbors(Agent);
#endif 
		Agent->Tick(DeltaTime);
		
	}
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	if (DebugRenderPartitions)
		pPartitionedSpace->RenderCells();
#endif
	if(DebugRenderNeighborhood)
	RenderNeighborhood();
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

  // TODO: implement ImGUI checkboxes for debug rendering here
		if (ImGui::Checkbox("first agent debug", &DebugRenderSteering))
		{
			Agents[0]->SetDebugRenderingEnabled(DebugRenderSteering);
		}
		ImGui::Checkbox("render neighborhood", &DebugRenderNeighborhood);
		bool bDebugRenderPartitions = DebugRenderPartitions;
		if (ImGui::Checkbox("Debug render partitions", &bDebugRenderPartitions))
		{
			DebugRenderPartitions = bDebugRenderPartitions;
		}
		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

  // TODO: implement ImGUI sliders for steering behavior weights here
		//float 
		ImGui::SliderFloat("Separation", &pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Cohesion", &pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("VelocityMatch", &pBlendedSteering->GetWeightedBehaviorsRef()[2].Weight, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Seek", &pBlendedSteering->GetWeightedBehaviorsRef()[3].Weight, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Wander", &pBlendedSteering->GetWeightedBehaviorsRef()[4].Weight, 0.0f, 1.0f, "%.2f");
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
	if (!DebugRenderNeighborhood)
		return;
	//DrawDebugCircle(pWorld, , NeighborhoodRadius, 8, FColor::Green);
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{

	NrOfNeighbors = 0;
	if (pAgent == nullptr)
		return;
	for (ASteeringAgent* Neighbor : Agents)
	{
		if (Neighbor == pAgent)
			continue;
		const float distance = FVector2D::Distance(pAgent->GetPosition(),Neighbor->GetPosition());
		if (distance < NeighborhoodRadius)
		{
			Neighbors[NrOfNeighbors] = Neighbor;
			++NrOfNeighbors;
		}
	}
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	const int TotalNeighbors = GetNrOfNeighbors();
	if (TotalNeighbors == 0)
		return FVector2D::ZeroVector;
	const auto& neighbors = GetNeighbors();
	FVector2D avgPosition = FVector2D::ZeroVector;
	for (int i{}; i < TotalNeighbors; ++i)
	{
		avgPosition += neighbors[i]->GetPosition();
	}
	avgPosition /= static_cast<float>(TotalNeighbors);
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	const int TotalNeighbors = GetNrOfNeighbors();
	if (TotalNeighbors == 0)
		return FVector2D::ZeroVector;
	const auto& neighbors = GetNeighbors();
	FVector2D avgVelocity = FVector2D::ZeroVector;

	for (int i{}; i < TotalNeighbors; ++i)
	{
		avgVelocity += neighbors[i]->GetLinearVelocity();
	}
	avgVelocity /= static_cast<float>(TotalNeighbors);

	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
	pSeekBehavior->SetTarget(Target);
}
int Flock::GetNrOfNeighbors() const
{
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	return pPartitionedSpace->GetNrOfNeighbors();
#else
	return NrOfNeighbors;
#endif
}
const TArray<ASteeringAgent*>& Flock::GetNeighbors() const
{
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	return pPartitionedSpace->GetNeighbors();
#else
	return Neighbors;
#endif
}

