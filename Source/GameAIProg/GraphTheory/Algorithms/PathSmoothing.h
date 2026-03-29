#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
{
public:
	//=== SSFA Functions ===
	//--- References ---
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
	static std::vector<NavLine> FindPortals(std::vector<Node*> const & Path, TriPolygon const & NavPoly)
	{
		//Container
		std::vector<NavLine> Portals = {};
		Portals.push_back(NavLine(Path[0]->GetPosition(), Path[0]->GetPosition()));

		//For each node received, get it's corresponding line
		for (size_t idx = 1; idx < Path.size(); ++idx)
		{
			auto pNode = Path[idx];
			auto pLine = NavPoly.GetEdges()[pNode->GetId()];

			//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
			FVector2D centerLine = FVector2D(pLine.GetP1(NavPoly) + pLine.GetP2(NavPoly))/2.f;
			auto previousPos = idx == 0 ? Path[0]->GetPosition() : Path[idx - 1]->GetPosition();
			auto cp = FVector2D::CrossProduct((centerLine - previousPos), (FVector2D{ pLine.GetP1(NavPoly) } - previousPos));

			NavLine portalLine{};
			if (cp > 0)//left;
				portalLine = NavLine(FVector2D{ pLine.GetP2(NavPoly) }, FVector2D{ pLine.GetP1(NavPoly) });
			else //right
				portalLine = NavLine(FVector2D{ pLine.GetP1(NavPoly) }, FVector2D{ pLine.GetP2(NavPoly) });

			//Store portal
			Portals.push_back(NavLine(portalLine));
		}

		//Add degenerate portal to force end evaluation
		Portals.push_back(NavLine(Path[Path.size() - 1]->GetPosition(), Path[Path.size() - 1]->GetPosition()));

		return Portals;
	}

	static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & Portals, TriPolygon const & NavPoly)
	{
		std::vector<FVector2D> Path{};
		const unsigned int amtOfPortals{ static_cast<unsigned int>(Portals.size()) };
		int apexIdx{ 0 }, leftLegIdx{ 1 }, rightLegIdx{ 2 };

		FVector2D apexPos{ Portals[apexIdx].P1 };
		FVector2D rightLeg{ Portals[rightLegIdx].P1 - apexPos };
		FVector2D leftLeg{ Portals[leftLegIdx].P2 - apexPos };

		Path.push_back(apexPos);
		//P1 == right point of portal, P2 == left point of portal
		for (unsigned int portalIdx = 1; portalIdx < amtOfPortals; ++portalIdx)
		{
			auto currentPortal = Portals[portalIdx];
			//--- RIGHT CHECK ---
			//1. See if moving funnel inwards - RIGHT
			FVector2D newRightLeg{ Portals[portalIdx].P1 - apexPos };
				//2. See if new line degenerates a line segment - RIGHT
			if (FVector2D::CrossProduct(rightLeg, newRightLeg) <= 0)
			{
				if (FVector2D::CrossProduct(leftLeg, newRightLeg) > 0)
				{
					rightLeg = newRightLeg;
					rightLegIdx = portalIdx;
				}
				else
				{
					//Leftleg becomes new apex point

					//Calculate new legs (if not the end)
					apexPos += leftLeg;
					apexIdx = leftLegIdx;
					portalIdx = leftLegIdx + 1;
					leftLegIdx = portalIdx;
					rightLegIdx = portalIdx;
					Path.push_back(apexPos);
					if (portalIdx < amtOfPortals)
					{
						rightLeg = Portals[rightLegIdx].P1 - apexPos;
						leftLeg = Portals[leftLegIdx].P2 - apexPos;
						continue;
					}
				}
			}
			//--- LEFT CHECK ---
			//1. See if moving funnel inwards - LEFT
			
				//2. See if new line degenerates a line segment - LEFT
			
					//Rightleg becomes new apex point
			
					//Calculate new legs (if not the end)
			FVector2D newLeftLeg{ Portals[portalIdx].P2 - apexPos };

			if (FVector2D::CrossProduct(leftLeg, newLeftLeg) >= 0)
			{
				if (FVector2D::CrossProduct(rightLeg, newLeftLeg) < 0)
				{
					leftLeg = newLeftLeg;
					leftLegIdx = portalIdx;
				}
				else
				{

					apexPos += rightLeg;
					apexIdx = rightLegIdx;
					portalIdx = rightLegIdx + 1;
					leftLegIdx = portalIdx;
					rightLegIdx = portalIdx;
					Path.push_back(apexPos);
					if (portalIdx < amtOfPortals)
					{
						rightLeg = Portals[rightLegIdx].P1 - apexPos;
						leftLeg = Portals[leftLegIdx].P2 - apexPos;
						continue;
					}
				}

			}
		  }

		Path.push_back(Portals.back().P1);

		// Add last path point

		return Path;
	}
private:
	SSFA() {};
	~SSFA() {};
};
}
