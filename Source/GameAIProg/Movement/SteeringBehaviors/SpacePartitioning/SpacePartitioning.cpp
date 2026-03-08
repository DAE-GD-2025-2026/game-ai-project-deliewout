#include "SpacePartitioning.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
{
	Neighbors.SetNum(MaxEntities);
	
	//calculate bounds of a cell
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;

	// TODO create the cells
	CellOrigin = { -Width / 2.f,-Height / 2.f };
	for (int row{}; row < NrOfRows; ++row)
	{
		for (int col{}; col < NrOfCols; ++col)
		{
			const float left = CellOrigin.X + CellWidth * col;
			const float bottom = CellOrigin.Y + CellHeight * row;
			Cells.push_back(Cell(left, bottom, CellWidth, CellHeight));
		}
	}
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	// TODO Add the agent to the correct cell
	int index = PositionToIndex(Agent.GetPosition());
	Cells[index].Agents.push_back(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	//TODO Check if the agent needs to be moved to another cell.
	//TODO Use the calculated index for oldPos and currentPos for this
	int OldIdx = PositionToIndex(OldPos);
	int CurIdx = PositionToIndex(Agent.GetPosition());
	if (OldIdx == CurIdx)
	{
		return;
	}
	Cells[OldIdx].Agents.remove(&Agent);
	Cells[CurIdx].Agents.emplace_back(&Agent);
}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius)
{
	// TODO Register the neighbors for the provided agent
	// TODO Only check the cells that are within the radius of the neighborhood
	NrOfNeighbors = 0;
	const FVector2D AgentPos{ Agent.GetPosition() };

	FRect QueryRect;
	QueryRect.Min = { AgentPos.X - QueryRadius,AgentPos.Y - QueryRadius };
	QueryRect.Max = { AgentPos.X + QueryRadius, AgentPos.Y + QueryRadius };

	for (const Cell& CurrentCell : Cells)
	{
		if (DoRectsOverlap(CurrentCell.BoundingBox, QueryRect))
		{
			for (ASteeringAgent* other : CurrentCell.Agents)
			{
				if (other == &Agent)
					continue;
				const float distSq = (other->GetPosition() - AgentPos).SizeSquared();
				if (distSq < QueryRadius * QueryRadius)
				{
					Neighbors[NrOfNeighbors] = other;
					++NrOfNeighbors;
				}
			}
		}
	}

}

void CellSpace::EmptyCells()
{
	for (Cell& c : Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	// TODO Render the cells with the number of agents inside of it
	for (const Cell& currentCell : Cells)
	{
		//draw the boxes
		FVector min(currentCell.BoundingBox.Min.X, currentCell.BoundingBox.Min.Y, 0.f);
		FVector max(currentCell.BoundingBox.Max.X, currentCell.BoundingBox.Max.Y, 0.f);

		FVector center = (min + max) / 2.0f;
		FVector extent = (max - min) / 2.0f;

		DrawDebugBox(pWorld,center,extent,FColor::Blue,false,0.f,0,1.f);

		// Draw the number of agents in the cell at the center
		FVector2D center2D = (currentCell.BoundingBox.Min + currentCell.BoundingBox.Max) * 0.5f;
		FVector center3D(center2D.X, center2D.Y, 0.f);

		DrawDebugString(
			pWorld,
			center3D,
			FString::Printf(TEXT("%d"), static_cast<int>(currentCell.Agents.size())),
			nullptr,
			FColor::Blue,
			0);
	}
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{
	// TODO Calculate the index of the cell based on the position
	FVector2D AgentPos{ Pos - CellOrigin };
	int col = std::clamp(static_cast<int>(AgentPos.X / CellWidth), 0, NrOfCols - 1);
	int row = std::clamp(static_cast<int>(AgentPos.Y / CellHeight), 0, NrOfRows - 1);
	int index = row * NrOfCols + col;
	return index;
}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}