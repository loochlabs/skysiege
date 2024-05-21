// Property of Looch Labs LLC.


#include "SkyGrid.h"

#include "GridCellActor.h"
#include "GridUnitActor.h"
#include "UnitTemplate.h"


void ASkyGrid::Setup(const FGridConfig& InGridConfig)
{
	Type = InGridConfig.Type;
	SetActorLocation(InGridConfig.WorldLocation);
	SetActorRotation(InGridConfig.WorldRotation);

	// hard coding grid spawning
	for(int32 c = 0; c < InGridConfig.Cols; ++c)
	{
		for(int32 r = 0; r < InGridConfig.Rows; ++r)
		{
			CreateCell(r, c);
		}
	}

	// padding
	for(int32 c = -InGridConfig.CellPadding; c < InGridConfig.CellPadding + InGridConfig.Cols; ++c)
	{
		for(int32 r = -InGridConfig.CellPadding; r < InGridConfig.CellPadding + InGridConfig.Rows; ++r)
		{
			if(GetCell(r, c))
				continue;

			CreateCell(r, c);
			AGridCellActor* cell = GetCell(r, c);
			cell->SetSelectable(false);
		}
	}
}

void ASkyGrid::Teardown()
{
	for(auto& cellPair : Cells)
	{
		cellPair.Value->Teardown();
	}
	Cells.Empty();
	Destroy();
}

void ASkyGrid::SetViewToMacro()
{
	SetViewToMacroBP();
}

void ASkyGrid::CreateCell(int32 row, int32 col)
{
	FCoordinates coord;
	coord.Row = row;
	coord.Col = col;

	check(CellClass);
	AGridCellActor* cell = GetWorld()->SpawnActor<AGridCellActor>(CellClass, GetActorTransform());
	check(cell);
	checkf(!Cells.Contains(coord), TEXT("Coord [%d, %d]"), coord.Row, coord.Col);
	Cells.Add(coord, cell);
	cell->Setup(this, row, col);
	CreateCellBP(cell);
}

void ASkyGrid::Cleanup()
{
	for(auto& cell : Cells)
		cell.Value->BeginCleanup();

	Cells.Empty();
	CleanupBP();
}

AGridCellActor* ASkyGrid::GetCell(int32 row, int32 col)
{
	FCoordinates coord;
	coord.Row = row;
	coord.Col = col;
	return Cells.Contains(coord) ? Cells[coord] : nullptr;
}

void ASkyGrid::GetCellShape(TArray<AGridCellActor*>& Out, int32 Row, int32 Col, const TArray<FCoordinates>& Shape)
{
	for(auto& coord : Shape)
	{
		int32 row = Row + coord.Row;
		int32 col = Col + coord.Col;
		AGridCellActor* cell = GetCell(row, col);
		if(cell)
			Out.Add(cell);
	}	
}

AGridCellActor* ASkyGrid::FindValidCell(AGridUnitActor* Unit)
{
	check(Unit);
	auto shape = Unit->GetOrientedShape();
	for(auto& cell : Cells)
	{
		TArray<AGridCellActor*> unitCells;
		GetCellShape(unitCells, cell.Key.Row, cell.Key.Col, shape);
		auto& unitTemplate = Unit->GetTemplate();

		bool bValid = true;
		for(auto& c : unitCells)
		{
			bValid &= c->CanBuild(unitTemplate);
		}
		if(bValid)
		{
			return unitCells[0];
		}
	}
	return nullptr;
}

bool ASkyGrid::IsGridUnitEmpty(int32 row, int32 col)
{
	AGridCellActor* cell = GetCell(row, col);
	return cell->UnitActors.IsEmpty();
}

bool ASkyGrid::GetRandomEmptyCell(int32& outR, int32& outC)
{
	// Get cells, shuffling
	TArray<AGridCellActor*> cells;
	Cells.GenerateValueArray(cells);
	for(int32 idx = 0; idx < cells.Num(); ++idx)
	{
		int32 rng = FMath::Rand() % cells.Num();
		auto& temp = cells[idx];
		cells[idx] = cells[rng];
		cells[rng] = temp;
	}

	for(auto& cell : cells)
	{
		if(IsGridUnitEmpty(cell->Row, cell->Col))
		{
			outR = cell->Row;
			outC = cell->Col;
			return true;
		}
	}
	return false;
}

void ASkyGrid::SetFocus(int32 Row, int32 Col)
{
	Focus.Row = Row;
	Focus.Col = Col;
	ClearAllHighlights();
	AGridCellActor* cell = GetCell(Row, Col);
	check(cell);
	cell->FocusUnit();
	OnFocused.Broadcast(cell);
}

void ASkyGrid::CycleFocus()
{
	FocusIndex++;
	AGridCellActor* cell = GetCell(Focus.Row, Focus.Col);
	cell->FocusUnit();
}

void ASkyGrid::Interact(int32 Row, int32 Col)
{
	AGridCellActor* cell = GetCell(Row, Col);
	check(cell);
	OnInteract.Broadcast(cell);
}

void ASkyGrid::ClearAllHighlights()
{
	for(auto& cellPair : Cells)
	{
		cellPair.Value->EndFocus();
	}
}

void ASkyGrid::RefreshHighlights()
{
	ClearAllHighlights();
	for(auto& cellPair : Cells)
	{
		if(cellPair.Key == Focus)
			cellPair.Value->BeginFocus();
	}
}

bool ASkyGrid::PlaceUnit(AGridUnitActor* Unit, int32 Row, int32 Col)
{
	check(Unit);
	AGridCellActor* origin = GetCell(Row, Col);
	if(!origin)
		return false;

	auto coords = Unit->GetOrientedShape();
	TArray<AGridCellActor*> cells;
	GetCellShape(cells, Row, Col, coords);

	auto& unitTemplate = Unit->GetTemplate();
	for(auto& c : cells)
	{
		if(!c->CanBuild(unitTemplate))
			return false;
	}

	// confirm placement
	for(auto& c : cells)
	{
		c->InsertUnit(Unit);
	}
	Unit->SetOriginCell(origin);
	RefreshUnitBonuses();
	RefreshHighlights();
	return true;
}

void ASkyGrid::RefreshUnitBonuses()
{
	TArray<AGridUnitActor*> units;
	for(auto& cellPair : Cells)
	{
		for(auto& unit : cellPair.Value->UnitActors)
		{
			units.AddUnique(unit);
		}
	}

	//apply bonuses
	for(auto& unit : units)
	{
		auto& unitBP = unit->GetTemplate();
		for(auto& bonusCfg : unitBP.BonusConfig)
		{
			auto buffCoords = unit->GetOrientedCoords(bonusCfg.Coords);
			TArray<AGridCellActor*> cells;
			GetCellShape(cells, unit->OriginGridCell->Row, unit->OriginGridCell->Col, buffCoords);
			for(auto& cell : cells)
			{
				for(auto& cellUnit : cell->UnitActors)
				{
					if(cellUnit->UnitTags.HasAny(bonusCfg.TagsRequiredToApply))
						cellUnit->UnitTags.AppendTags(bonusCfg.BonusTags);
				}
			}
		}
	}
}
