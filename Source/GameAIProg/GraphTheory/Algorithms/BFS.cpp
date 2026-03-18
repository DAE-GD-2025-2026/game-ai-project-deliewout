#include "BFS.h"


#include <queue>

#include "Shared/Graph/Graph.h"

using namespace GameAI;

BFS::BFS(Graph* const pGraph)
	: pGraph(pGraph)
{
}

// TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode) const
{
	std::vector<Node*> visited = {};
	std::queue<Node*> nodeQueue = {};//openlist
	nodeQueue.push(pStartNode);
	std::map<Node*, Node*> parent = {};//closedlist
	visited.push_back(pStartNode);
	while (!nodeQueue.empty())
	{
		Node* pCurrentNode = nodeQueue.front();
		nodeQueue.pop();
		if (pCurrentNode == pDestinationNode)
			return ReconstructPath(parent, pStartNode, pDestinationNode);
		for (Connection* const pConnection : pGraph->FindConnectionsFrom(pCurrentNode->GetId()))
		{
			Node* pNextNode = pGraph->GetNode(pConnection->GetToId()).get();
			if (parent.find(pCurrentNode) == parent.end())
			{
				visited.push_back(pNextNode);
				parent[pNextNode] = pCurrentNode;
				nodeQueue.push(pNextNode);
			}
		}
	}
	return std::vector<Node*>{};
}

std::vector<Node*> GameAI::BFS::ReconstructPath(std::map<Node*, Node*> parent, Node* pStartNode, Node* pEndNode) const
{
	Node* pCurrentNode = pEndNode;
	std::vector<Node*> path;
	while (pCurrentNode != pStartNode)
	{
		path.emplace_back(pCurrentNode);
		pCurrentNode = parent.at(pCurrentNode);
	}
	path.emplace_back(pStartNode);
	std::reverse(path.begin(), path.end());
	return path;
}
