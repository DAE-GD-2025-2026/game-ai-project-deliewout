#pragma once
#include <stack>
#include "Shared/Graph/Graph.h"

namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		//If the graph is not connected, there can be no Eulerian Trail
		if (m_pGraph->GetConnections().size() == 0)
			return Eulerianity::notEulerian;
		//Count nodes with odd degree 
		auto& Nodes = m_pGraph->GetNodes();
		int oddNodes=0;
		for (const auto& Node : Nodes)
		{
			int degree = 0;
			int nodeId = Node->GetId();

			// Count all connections where this node is either the source or the destination
			for (const auto& conn : m_pGraph->GetConnections())
			{
				if (conn->GetFromId() == nodeId || conn->GetToId() == nodeId)
					++degree;
			}

			if (degree % 2 != 0)
				++oddNodes;
		}
		//A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (oddNodes > 2)
			return Eulerianity::notEulerian;
		//A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		else if (oddNodes == 2 && Nodes.size() != 2)
			return Eulerianity::semiEulerian;
		else if (oddNodes == 0)
		{
			return Eulerianity::eulerian;
		}
		return Eulerianity::notEulerian;
		//An Euler trail can be made, but only starting and ending in these 2 nodes

		//A connected graph with no odd nodes is Eulerian
		
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		Node* StartNode = nullptr;
		
		// TODO Check if there can be an Euler path
		// TODO If this graph is not eulerian, return the empty path
		eulerianity = IsEulerian();
		if (eulerianity == Eulerianity::notEulerian)
			return Path;
		else if (eulerianity==Eulerianity::eulerian)
		{
			currentNodeId = Nodes[0]->GetId();
		}
		else if(eulerianity==Eulerianity::semiEulerian)
		{
			for (int i{}; i < m_pGraph->GetNodeCount(); i++)
			{
				auto connections = m_pGraph->FindConnectionsFrom(i);
				if (connections.size() % 2 != 0)
				{
					currentNodeId = i;
					break;
				}
			}
		}

		// TODO Start algorithm loop
		std::stack<int> nodeStack;
		while (!nodeStack.empty() || !graphCopy.FindConnectionsFrom(currentNodeId).empty() )
		{
			if (!graphCopy.FindConnectionsFrom(currentNodeId).empty())
			{
				nodeStack.push(currentNodeId);
				int neighbor = graphCopy.FindConnectionsFrom(currentNodeId)[0]->GetToId();
				graphCopy.RemoveConnection(currentNodeId, neighbor);
				currentNodeId = neighbor;
				//grap
			}
			else
			{
				Path.push_back(m_pGraph->GetNode(currentNodeId).get());
				currentNodeId = nodeStack.top();
				nodeStack.pop();
			}

		}
		Path.push_back(m_pGraph->GetNode(currentNodeId).get());
		std::reverse(Path.begin(), Path.end());
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		//Mark the visited node
		visited[startIndex] = true;
		//Ask the graph for the connections from that node
		std::vector<Connection*> NodeConnections = m_pGraph->FindConnectionsFrom(startIndex);
		//recursively visit any valid connected nodes that were not visited before
		//Tip: use an index-based for-loop to find the correct index
		for (int i{ 0 }; i < NodeConnections.size(); i++)
		{
			int ConnectionsIndex = NodeConnections[i]->GetFromId();
			if (!visited[ConnectionsIndex])
			{
				visited[ConnectionsIndex] = true;
				VisitAllNodesDFS(Nodes, visited, ConnectionsIndex);
			}
		}
	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		std::vector<bool> Visited{};
		if (Nodes.size() == 0)
			return false;

		//choose a starting node
		
		//start a depth-first-search traversal from the node that has at least one connection
		VisitAllNodesDFS(Nodes, Visited, 0);
		//if a node was never visited, this graph is not connected
		for (bool NodeVisited : Visited)
		{
			if (!NodeVisited)
				return false;
		}
		return true;
	}
}