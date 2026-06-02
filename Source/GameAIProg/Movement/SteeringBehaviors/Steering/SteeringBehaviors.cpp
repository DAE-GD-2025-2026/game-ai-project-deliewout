#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
//*******

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	Steering.LinearVelocity *= -1;
	return Steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	const float MaxLineairSpeed{ Agent.GetMaxLinearSpeed() };
	Steering.LinearVelocity =  Agent.GetPosition()- Target.Position ;
	//TODO: solve the problem in the distance calculation
	const float Distance = Steering.LinearVelocity.Length() - TargetRadius;
	if (Distance < SlowRadius)
	{
		Steering.LinearVelocity *= Agent.GetMaxLinearSpeed() * (Distance / (SlowRadius + TargetRadius));
	}
	else
	{
		Steering.LinearVelocity *= Agent.GetMaxLinearSpeed();
	}
	if (Agent.GetDebugRenderingEnabled())
	{	
		//had to rotate the circle so its on the floor instead of facing upwards
		//DrawDebugDirectionalArrow(Agent.GetWorld(), Agent.GetActorLocation(), A, 100, FColor(125U, 125U, 0U));
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), SlowRadius, 8, FColor(255U, 0U, 0U),false,-1.f,0U,0.0f,FVector(1.0f,0.0f,0.0f),FVector(0.0f,1.0f,0.0f));
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), TargetRadius, 8, FColor(0U, 255U, 0U), false, -1.f, 0U, 0.0f, FVector(1.0f, 0.0f, 0.0f), FVector(0.0f, 1.0f, 0.0f));
	}
	return Steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Agent.SetIsAutoOrienting(false);

	FVector2D Direction = Target.Position - Agent.GetPosition();
	Direction.Normalize();

	float RotationAngle = FMath::Atan2(Direction.Y, Direction.X);
	float AngleDiffrence = RotationAngle - Agent.GetRotation();

	Steering.AngularVelocity = AngleDiffrence * DeltaT;

	return Steering;
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	float Distance = (Target.Position - Agent.GetPosition()).Length();
	float TargetTime = Distance / Agent.GetMaxLinearSpeed();
	FVector2D PredictedPos = Target.Position + Target.LinearVelocity * TargetTime;
	FVector2D Direction = PredictedPos - Agent.GetPosition();
	Direction.Normalize();
	Steering.LinearVelocity = Direction * Agent.GetMaxLinearSpeed();
	return Steering;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	float Distance = (Target.Position - Agent.GetPosition()).Length();
	if (Distance < EvadeRadius)
	{
		float TargetTime = Distance / Agent.GetMaxLinearSpeed();
		FVector2D PredictedPos = Target.Position + Target.LinearVelocity * TargetTime;
		FVector2D Direction = PredictedPos - Agent.GetPosition();
		Direction.Normalize();
		Steering.LinearVelocity = -(Direction * Agent.GetMaxLinearSpeed());
	}

	if (Agent.GetDebugRenderingEnabled())
	{
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), EvadeRadius, 8, FColor::Red, false, -1.f, 0, 0.0f, FVector(1.0f, 0.0f, 0.0f), FVector(0.0f, 1.0f, 0.0f));
	}
	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	OffsetDistance = Radius * 2.f / 3.f;
	SteeringOutput Steering{};
	FVector Forward = Agent.GetActorForwardVector();
	FVector2D ForwardVector{ Forward.X,Forward.Y };
	ForwardVector.Normalize();

	FVector AgentLocation3D = Agent.GetActorLocation();
	FVector2D AgentLocation2D{AgentLocation3D.X, AgentLocation3D.Y};
	FVector2D CircleCenter = AgentLocation2D + ForwardVector * OffsetDistance;

	float angleChange = FMath::FRandRange(-MaxAngleChange, MaxAngleChange);
	WanderAngle += angleChange;
	FVector2D OnCircle{ FMath::Cos(WanderAngle),FMath::Sin(WanderAngle) };
	FVector2D wanderTarget = CircleCenter + OnCircle * Radius;

	FVector2D dir = wanderTarget - Agent.GetPosition();
	if (!dir.IsNearlyZero())
	{
		dir.Normalize();
		Steering.LinearVelocity = dir * Agent.GetMaxLinearSpeed();
	}

	if (Agent.GetDebugRenderingEnabled())
	{
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation()+OffsetDistance, Radius, 8, FColor::Red, false, -1.f, 0, 0.0f, FVector(1.0f, 0.0f, 0.0f), FVector(0.0f, 1.0f, 0.0f));

	}
	return Steering;
}
