
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering = {};
	float totalWeight = { 0.f };
	//TODO: Calculate the weighted average steeringbehavior
	for (const BlendedSteering::WeightedBehavior& weightedBehaviour : WeightedBehaviors)
	{
		SteeringOutput Steering = weightedBehaviour.pBehavior->CalculateSteering(DeltaT, Agent);
		BlendedSteering.LinearVelocity = Steering.LinearVelocity * weightedBehaviour.Weight;
		BlendedSteering.AngularVelocity = Steering.AngularVelocity * weightedBehaviour.Weight;
		totalWeight += weightedBehaviour.Weight;
	}

	if (totalWeight > 0.f)
	{
		BlendedSteering /= totalWeight;
	}

	if (Agent.GetDebugRenderingEnabled())
		DrawDebugDirectionalArrow(
			Agent.GetWorld(),
			Agent.GetActorLocation(),
			Agent.GetActorLocation() + FVector{BlendedSteering.LinearVelocity, 0} * (Agent.GetMaxLinearSpeed() * DeltaT),
			30.f, FColor::Red
			);

	return BlendedSteering;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);

		if (Steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return Steering;
}