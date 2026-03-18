#include "BFS.h"

#include <map>
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
	std::vector<Node*> path = {};
	std::queue<Node*> openList = {};
	std::map<Node*, Node*> closedList = {};
	openList.push(pStartNode);
	while (!openList.empty())
	{
		Node* pCurrentNode = openList.front();
		openList.pop();
		if (pCurrentNode == pDestinationNode)
			return ReconstructPath(closedList, pStartNode, pDestinationNode);
		//for(Connection* const)
	}
	return path;
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
