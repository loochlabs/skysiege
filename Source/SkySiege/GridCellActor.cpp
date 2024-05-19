// Property of Looch Labs LLC.


#include "GridCellActor.h"
#include "GridUnitActor.h"
#include "SkyGameInstance.h"
//#include "SessionProfile.h" @CLEAN
#include "UnitTemplate.h"
#include "CellHighlights.h"

void AGridCellActor::Setup(int32 row, int32 col)
{
	check(Row == -1);
	check(Col == -1);
	Row = row;
	Col = col;
	check(UnitActors.IsEmpty());
	UnitActors.Empty();
}

void AGridCellActor::Teardown()
{
	for(AGridUnitActor* unit : UnitActors)
	{
		unit->Destroy();
	}
	UnitActors.Empty();
	Destroy();
}

void AGridCellActor::SetSelectable(bool bEnable)
{
	bSelectable = bEnable;
	OnUpdatedSelectable();
}

FVector AGridCellActor::GetUnitAnchorLocation()
{
	return GetActorLocation() + UnitAnchorLocation;
}

// Handle any animations and follow up destroy. This is effectively a "set it and forget it" cleanup from the Grid
void AGridCellActor::BeginCleanup()
{
	BeginCleanupBP();
}

void AGridCellActor::Cleanup()
{
	//@TODO do we need this?
	//if(UnitActor)
	//	UnitActor->Destroy();

	Destroy();
}

bool AGridCellActor::CanBuild(const FUnitTemplate& UnitTemplate)
{
	if(!bSelectable)
		return false;

	if(!UnitTemplate.TagsRequiredToBuild.IsEmpty() && !HasUnitTags(UnitTemplate.TagsRequiredToBuild))
		return false;

	if(!UnitTemplate.TagsBlockingBuild.IsEmpty() && HasUnitTags(UnitTemplate.TagsBlockingBuild))
		return false;

	return true;
}

bool AGridCellActor::HasUnitTags(const FGameplayTagContainer& InTags)
{
	bool bHasTags = false;
	for(auto& unit : UnitActors)
	{
		bHasTags |= unit->UnitTags.HasAny(InTags);
	}
	return bHasTags;
}

void AGridCellActor::BeginHighlight()
{
	//@CLEAN
	//USessionProfile* user = USessionManager::GetUserProfile(this);
	//user->BeginCellHighlight(Row, Col);
}

void AGridCellActor::EndHighlight()
{
	Highlight = ECellHighlight::None;
	EndHighlightBP();
}

void AGridCellActor::RefreshHighlight()
{
	if(Highlight == ECellHighlight::None) return;

	BeginHighlight();
}

void AGridCellActor::SetHighlight(ECellHighlight InHighlight)
{
	Highlight = InHighlight;
	BeginHighlightBP();
}

void AGridCellActor::Interact()
{
	//@CLEAN
	//USessionProfile* user = USessionManager::GetUserProfile(this);
	//user->TryToInteract(Row,Col);
}

void AGridCellActor::InsertUnit(AGridUnitActor* Unit)
{
	UnitActors.Add(Unit);
}

void AGridCellActor::RemoveUnit(AGridUnitActor* Unit)
{
	UnitActors.Remove(Unit);
}

