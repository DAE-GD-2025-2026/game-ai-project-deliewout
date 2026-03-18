#pragma once
#include <vector>
#include <map>

namespace GameAI
{
	class Graph;
	class Node;
	class BFS
	{
	public:
		BFS(Graph* const pGraph);

		std::vector<Node*> FindPath(Node* const pStartNode, Node* const pDestinationNode) const;

	private:
		Graph* pGraph;
		std::vector<Node*> ReconstructPath(std::map<Node*, Node*> parent, Node* pStartNode, Node* pEndNode) const;
	};
}
