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
	Neighbors.SetNum(FlockSize);
	//initialize the flock and the memory pool
	
	for (int i{ 0 }; i < FlockSize; ++i)
	{
		ASteeringAgent* agent = pWorld->SpawnActor<ASteeringAgent>(AgentClass, FVector{ FMath::FRandRange(-WorldSize, WorldSize),FMath::FRandRange(-WorldSize, WorldSize),90.0f }, FRotator::ZeroRotator);
		Agents[i]= agent;
		//if (Agents[i] == nullptr)
		//	--i;
	}

	pSeparationBehavior = std::make_unique<Separation>(this);
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pVelMatchBehavior = std::make_unique<VelocityMatch>(this);
	pSeekBehavior = std::make_unique<Seek>(this);
	pWanderBehavior = std::make_unique<Wander>(this);
}

Flock::~Flock()
{
 // TODO: Cleanup any additional data
	Neighbors.Empty();
	Agents.Empty();
}

void Flock::Tick(float DeltaTime)
{
 // TODO: update the flock
 // TODO: for every agent: done
  // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent) done
  // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool) done
  // TODO: trim the agent to the world
	for (ASteeringAgent* Agent : Agents)
	{
		RegisterNeighbors(Agent);
		Agent->Tick(DeltaTime);
		FVector2D pos = Agent->GetPosition();

	}
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
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

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

  // TODO: implement ImGUI sliders for steering behavior weights here
		//float 
		//ImGui::SliderFloat("Cohesion",)
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
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
		const float distance = Neighbor->GetPosition().Length() - pAgent->GetPosition().Length();
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
	FVector2D avgPosition = FVector2D::ZeroVector;
	for (const auto& Neighbor: Neighbors)
	{
		avgPosition += Neighbor->GetPosition();
	}
	avgPosition /= Neighbors.Num();
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;

	for (const auto& Neighbor : Neighbors)
	{
		avgVelocity += Neighbor->GetLinearVelocity();
	}
	avgVelocity /= Neighbors.Num();

	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
 // TODO: Implement
}

