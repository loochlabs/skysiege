// Property of Looch Labs LLC.


#include "GridUnitActor.h"

#include "Coordinates.h"
#include "GridCellActor.h"
#include "SkyGameInstance.h"
#include "SkyGameModeBase.h"
#include "UnitTemplate.h"
#include "Kismet/GameplayStatics.h"


void AGridUnitActor::Setup(const FName& InUnitKey)
{
	static int32 id = 0;
	UnitID = ++id;
	UnitKey = InUnitKey;
	check(!UnitKey.IsNone());

	auto& unitTemplate = GetTemplate();
	UnitTags = unitTemplate.UnitTags;
	
	OnSetupComplete();
}

void AGridUnitActor::Teardown()
{
	OriginGridCell->RemoveUnit(this);
	OriginGridCell->Grid->RefreshUnitBonuses();
	OriginGridCell->Grid->RefreshHighlights();
	OriginGridCell = nullptr;
	Destroy();
}

void AGridUnitActor::BeginDestroy()
{
	OriginGridCell = nullptr;
	Super::BeginDestroy();
}

const FUnitTemplate& AGridUnitActor::GetTemplate()
{
	return ASkyGameMode::Get(this)->GetUnitTemplate(UnitKey);
}

int32 AGridUnitActor::GetSellPrice()
{
	auto& unitTemplate = GetTemplate();
	return unitTemplate.ShopCost / 2 + 1;
}

void AGridUnitActor::RefreshLocation()
{
	FVector loc = OriginGridCell->GetUnitAnchorLocation();
	SetActorLocation(loc);
}

void AGridUnitActor::SetOriginCell(AGridCellActor* Cell)
{
	check(Cell);
	OriginGridCell = Cell;
	RefreshLocation();
}

void AGridUnitActor::Rotate(bool CW)
{
	switch(GridOrientation)
	{
	case ECellOrientation::North:	GridOrientation = ECellOrientation::East; break;  
	case ECellOrientation::East:	GridOrientation = ECellOrientation::South; break;  
	case ECellOrientation::South:	GridOrientation = ECellOrientation::West; break;  
	case ECellOrientation::West:	GridOrientation = ECellOrientation::North; break;

	default:
		check(false); //invalid orientation
	}
	
	OnUpdatedOrientation();
}

void AGridUnitActor::RotateTo(ECellOrientation InOrientation)
{
	while(GridOrientation != InOrientation)
	{
		Rotate();
	}
}

TArray<FCoordinates> AGridUnitActor::GetOrientedCoords(const TArray<FCoordinates>& Coords)
{
	TArray<FCoordinates> results = Coords;
	auto rotate = [&](int32 RotateCount)
	{
		for(int32 n = 0; n < RotateCount; ++n)
			for(auto& coord : results)
			{
				int32 temp = coord.Row;
				coord.Row = -coord.Col;
				coord.Col = temp;
			}
	};

	switch(GridOrientation)
	{
	case ECellOrientation::East:
		rotate(1);
		break;

	case ECellOrientation::South:
		rotate(2);
		break;

	case ECellOrientation::West:
		rotate(3);
		break;

	default:
		break;
	}
	return results;
}

TArray<FCoordinates> AGridUnitActor::GetOrientedShape()
{
	auto& unitTemplate = GetTemplate();
	check(unitTemplate.GridShape.Num() > 0);
	return GetOrientedCoords(unitTemplate.GridShape);
}




void AGridUnitActor::SetFocus(bool bFocused)
{
	OnUpdatedFocus(bFocused);
}

void AGridUnitActor::HandleBattleEvent(const FBattleEvent& Event)
{
	OnBattleEvent(Event);
}
