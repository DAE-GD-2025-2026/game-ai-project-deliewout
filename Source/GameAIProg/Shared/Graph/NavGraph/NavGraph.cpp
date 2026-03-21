#include "NavGraph.h"

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
	: Graph{false}
	, pNavPoly{std::move(NavPoly)}
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	Nodes.reserve(Other.Nodes.size());
	for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
	{
		Nodes.push_back(std::make_unique<NavGraphNode>(*dynamic_cast<NavGraphNode*>(OtherNode.get())));
	}
        
	Connections.reserve(Other.Connections.size());
	for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	if (EdgeIdx >= 0)
	{
		for (auto const & pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}
	
	return Graphs::InvalidNodeId;
}

void GameAI::NavGraph::CreateNavigationGraph()
{
	//1. Go over all the edges of the navigation mesh and create nodes
			// Create node here
	for (int lineIdx=0; lineIdx< pNavPoly->GetEdges().size();++lineIdx)
	{
		const auto triangles = GetTrianglesFromLineIndex(lineIdx);
		if (triangles.size() > 1)
		{
			const auto& line = pNavPoly->GetEdges()[lineIdx];
			const FVector2D& lineMiddle = FVector2D{ (line.GetP1(*pNavPoly.get()) + line.GetP2(*pNavPoly.get() ))} / 2;
			AddNode(std::make_unique<Node>(lineMiddle, lineIdx));
		}
	}
	//2. Create connections now that every node is created	
		//2 valid nodes -> 1 connection
		//3 valid nodes -> 3 connections
	for (const auto& triangle : pNavPoly->GetTriangles())
	{
		std::vector<int> validNodeIds{};
		for (const auto& edge : triangle.GetEdges())
		{
			const auto& edgeIdx = pNavPoly->FindEdgeIndex(edge);
			const auto& nodeId = GetNodeIdFromEdgeIndex(edgeIdx.value());
			if (GetNodeIdFromEdgeIndex(edgeIdx.value()) != Graphs::InvalidNodeId)
			{
				validNodeIds.push_back(nodeId);
			}
		}
		if (validNodeIds.size() == 2)
		{
			AddConnection(validNodeIds[0], validNodeIds[1]);
		}
		else if (validNodeIds.size() == 3)
		{
			AddConnection(validNodeIds[0], validNodeIds[1]);
			AddConnection(validNodeIds[1], validNodeIds[2]);
			AddConnection(validNodeIds[2], validNodeIds[0]);
		}
	}

		
	//3. Set the connections cost to the actual distance
	SetConnectionCostsToDistances();
}

const std::vector<const TriPolygon::Triangle*> GameAI::NavGraph::GetTrianglesFromLineIndex(const int lineIndex) const
{
	std::vector<const TriPolygon::Triangle*> result;
	for (const auto& triangle : pNavPoly->GetTriangles())
	{
		const auto& triangleEdges = triangle.GetEdges();
		for (const auto& triangleEdge : triangleEdges)
		{
			if (pNavPoly->FindEdgeIndex(triangleEdge) == lineIndex)
			{
				result.push_back(&triangle);
				break;
			}
		}
	}
	return result;
}
