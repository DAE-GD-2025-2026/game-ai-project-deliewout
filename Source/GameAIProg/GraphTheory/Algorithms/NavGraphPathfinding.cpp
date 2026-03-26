#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{
	//Create the path to return
	std::vector<FVector2D> finalPath{};

	//Get the start and endTriangle
	const auto startTriangle = pNavGraph->GetNavPolygon()->GetTriangleAtPosition(startPos,true);
	const auto endTriangle = pNavGraph->GetNavPolygon()->GetTriangleAtPosition(endPos,true);
	//We have valid start/end triangles and they are not the same
	if (!startTriangle || !endTriangle)
		return finalPath;
	if (startTriangle == endTriangle)
		return { startPos,endPos };

	//=> Start looking for a path
	//Copy the graph
	auto pNavGraphCopy = pNavGraph->Clone();

	//Create Extra node for the Start Node (Agent's position
	auto startNode= std::make_unique<NavGraphNode>(startPos,-1);
	const int startNodeId = startNode->GetId();
	pNavGraphCopy->AddNode(std::move(startNode));
	for (const auto& edge : startTriangle->GetEdges())
	{
		auto edgeIdx = pNavGraph->GetNavPolygon()->FindEdgeIndex(edge);
		if (!edgeIdx.has_value())
			continue;

		const int lineIdx = edgeIdx.value();
		const int otherNodeId = pNavGraphCopy->GetNodeIdFromEdgeIndex(lineIdx);
		if (otherNodeId != Graphs::InvalidNodeId)
		{
			const float cost = FVector2D::Distance(startPos, pNavGraphCopy->GetNode(otherNodeId)->GetPosition());
			auto connection = std::make_unique<Connection>(startNodeId, otherNodeId);
			connection->SetWeight(cost);
			pNavGraphCopy->AddConnection(std::move(connection));
		}
	}
	//Create extra node for the endNode
	auto endNode = std::make_unique<NavGraphNode>( startPos,pNavGraphCopy->GetNodes().size());
	const int endNodeId = endNode->GetId();
	pNavGraphCopy->AddNode(std::move(endNode));
	for (const auto& edge : endTriangle->GetEdges())
	{
		auto edgeIdx = pNavGraph->GetNavPolygon()->FindEdgeIndex(edge);
		if (!edgeIdx.has_value())
			continue;

		const int lineIdx = edgeIdx.value();
		const int otherNodeId = pNavGraphCopy->GetNodeIdFromEdgeIndex(lineIdx);
		if (otherNodeId != Graphs::InvalidNodeId)
		{
			const float cost = FVector2D::Distance(endPos, pNavGraphCopy->GetNode(otherNodeId)->GetPosition());
			auto connection = std::make_unique<Connection>(endNodeId, otherNodeId);
			connection->SetWeight(cost);
			pNavGraphCopy->AddConnection(std::move(connection));
		}
	}
	//Run A star on new graph
	AStar aStar{ pNavGraphCopy.get(),HeuristicFunctions::Euclidean };
	auto path = aStar.FindPath(pNavGraphCopy->GetNode(startNodeId).get(), pNavGraphCopy->GetNode(endNodeId).get());
	finalPath.reserve(path.size());
	for (auto& currentNode : path)
	{
		finalPath.emplace_back(currentNode->GetPosition());
	}
	//Debug Visualisation

	// Extra: Run optimiser on new graph (First check if everything works without SSFA!)
	// debugPortals = SSFA::FindPortals(nodes, *pNavGraph->GetNavPolygon());
	// finalPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
	
	return finalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}