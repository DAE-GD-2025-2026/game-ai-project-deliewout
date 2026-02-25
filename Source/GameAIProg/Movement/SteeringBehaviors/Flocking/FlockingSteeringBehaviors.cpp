#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};
	FVector2D targetPos = pFlock->GetAverageNeighborPos();
	if (targetPos == FVector2D::ZeroVector)
		return Steering;
	Steering.LinearVelocity = targetPos - pAgent.GetPosition();
	Steering.LinearVelocity.Normalize();
	Steering.LinearVelocity *= pAgent.GetMaxLinearSpeed();
	return Steering;
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};
	FVector2D SeperationForce;
	for (const ASteeringAgent* Agent : pFlock->GetNeighbors())
	{
		if (!Agent)continue;
		FVector2D TargetPos = pAgent.GetPosition() - Agent->GetPosition();
		float Magnitude = TargetPos.Length();
		if (Magnitude>0)
		{
			SeperationForce = (TargetPos * Magnitude) / (1 / Magnitude);
		}
	}
	Steering.LinearVelocity = SeperationForce;
	Steering.LinearVelocity.Normalize();
	Steering.LinearVelocity *= pAgent.GetMaxLinearSpeed();
	return Steering;
}
//*************************
//VELOCITY MATCH (FLOCKING)

SteeringOutput VelocityMatch::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput Steering{};
	Steering.LinearVelocity = pFlock->GetAverageNeighborVelocity();
	Steering.LinearVelocity.Normalize();
	Steering.LinearVelocity *= pAgent.GetMaxLinearSpeed();
	return Steering;
}
