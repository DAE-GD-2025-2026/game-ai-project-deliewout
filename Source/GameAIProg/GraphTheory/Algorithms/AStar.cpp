#include "AStar.h"

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	std::vector<Node*> path{};
	std::vector<NodeRecord> openList{};
	std::vector<NodeRecord> closedList{};
	NodeRecord currentNodeRecord{};
	const NodeRecord startRecord{ pStartNode };
	openList.push_back(startRecord);

	while (!openList.empty())
	{
		currentNodeRecord = *std::min_element(openList.begin(), openList.end());
		if (pGoalNode == currentNodeRecord.pNode)
			break;
		const std::vector<Connection*> connections = pGraph->FindConnectionsFrom(currentNodeRecord.pNode->GetId());
		for ( Connection* currentConnection : connections)
		{
			const float currentGCost = currentNodeRecord.costSoFar + currentConnection->GetWeight();
			//TODO check if it should be connection->GetCost()
			//NOTE the getweight should be the same as the GetWeight
			Node* pNextNode = pGraph->GetNode(currentConnection->GetToId()).get();

			auto nodeInClosedList = std::find_if(closedList.begin(), closedList.end(),
				[pNextNode](const NodeRecord& r) {return r.pNode == pNextNode; });

			if (nodeInClosedList != closedList.end())
			{
				if (nodeInClosedList->costSoFar <= currentGCost)
					continue;
				closedList.erase(nodeInClosedList);
			}

			auto nodeInOpenList = std::find_if(openList.begin(), openList.end(),
				[pNextNode](const NodeRecord& r) {return r.pNode == pNextNode; });

			if (nodeInOpenList != openList.end())
			{
				if (nodeInOpenList->costSoFar <= currentGCost)
					continue;
				openList.erase(nodeInOpenList);
			}
			openList.push_back(NodeRecord{ pNextNode,currentConnection,currentGCost,currentGCost + GetHeuristicCost(pNextNode,pGoalNode) });

		}
		//go to trough an iterator or else it wouldnt find the noderecord
		auto currentIt = std::find(openList.begin(), openList.end(), currentNodeRecord);
		if (currentIt != openList.end())
		{
			openList.erase(currentIt);
		}
		closedList.push_back(currentNodeRecord);
	}

	//extra assignment: fallback
	if (currentNodeRecord.pNode != pGoalNode)
	{
		auto closestIt = std::min_element(
			closedList.begin(), closedList.end(),
			[&](const NodeRecord& a, const NodeRecord& b)
			{
				return GetHeuristicCost(a.pNode, pGoalNode) < GetHeuristicCost(b.pNode, pGoalNode);
			}
		);
		currentNodeRecord = *closestIt;
	}
	while (currentNodeRecord.pNode != pStartNode)
	{
		path.push_back(currentNodeRecord.pNode);
		Node* pFromNode = pGraph->GetNode(currentNodeRecord.pConnection->GetFromId()).get();
		auto recordInClosedList = std::find_if(closedList.begin(), closedList.end(),
			[pFromNode](const NodeRecord& r) {return r.pNode == pFromNode; });
		if (recordInClosedList == closedList.end())
			break;
		currentNodeRecord = *recordInClosedList;
	}
	path.push_back(pStartNode);
	std::reverse(path.begin(), path.end());
	return path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}