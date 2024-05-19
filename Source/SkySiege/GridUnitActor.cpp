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

void AGridUnitActor::BeginDestroy()
{
	OriginGridCell = nullptr;
	Super::BeginDestroy();
}

const FUnitTemplate& AGridUnitActor::GetTemplate()
{
	ASkyGameMode* gamemode = Cast<ASkyGameMode>(UGameplayStatics::GetGameMode(this));
	return gamemode->GetUnitTemplate(UnitKey);
	
	//@CLEAN
	//UMineshaftGameInstance* gi = GetWorld()->GetGameInstance<UMineshaftGameInstance>();
	//USessionManager* sm = gi->SessionManager;
	//return sm->GetUnitBlueprint(UnitKey);
}

void AGridUnitActor::SetOriginCell(AGridCellActor* Cell)
{
	check(Cell);
	OriginGridCell = Cell;
	FVector loc = Cell->GetUnitAnchorLocation();
	SetActorLocation(loc);
}

TArray<FCoordinates> AGridUnitActor::GetOrientedCoords(const TArray<FCoordinates>& Coords)
{
	TArray<FCoordinates> result = Coords;

	auto rotate = [&](int32 RotateCount)
	{
		for(int32 n = 0; n < RotateCount; ++n)
		for(auto& coord : result)
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
	
	return result;
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

void AGridUnitActor::SetFocus(bool bFocused)
{
	OnUpdatedFocus(bFocused);
}

void AGridUnitActor::HandleBattleEvent(const FBattleEvent& Event)
{
	OnBattleEvent(Event);
}
