// Property of Looch Labs LLC.


#include "GridCellActor.h"
#include "GridUnitActor.h"
#include "UnitTemplate.h"
#include "CellHighlights.h"
#include "SkyGrid.h"

void AGridCellActor::Setup(ASkyGrid* ParentGrid, int32 InRow, int32 InCol)
{
	check(ParentGrid);
	check(Row == -1);
	check(Col == -1);
	Grid = ParentGrid;
	Row = InRow;
	Col = InCol;
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

void AGridCellActor::BeginFocus()
{
	Grid->SetFocus(Row, Col);
}

int32 AGridCellActor::GetUnitFocusIndex()
{
	return Grid->FocusIndex > 0 && !UnitActors.IsEmpty() ? Grid->FocusIndex % UnitActors.Num() : 0;
}

bool AGridCellActor::IsTopUnitFocusIndex()
{
	return UnitActors.IsEmpty() || Grid->FocusIndex % UnitActors.Num() == (UnitActors.Num()-1);
}

void AGridCellActor::FocusUnit()
{
	int32 unitIdx = GetUnitFocusIndex();
	BeginHighlightBP(unitIdx);
}

void AGridCellActor::EndFocus()
{
	Highlight = ECellHighlight::None;
	EndHighlightBP();
}

void AGridCellActor::RefreshHighlight()
{
	if(Highlight == ECellHighlight::None) return;

	BeginFocus();
}

void AGridCellActor::SetHighlight(ECellHighlight InHighlight)
{
	Highlight = InHighlight;
	int32 unitIdx = GetUnitFocusIndex();
	BeginHighlightBP(unitIdx);
}

void AGridCellActor::Interact()
{
	Grid->Interact(Row, Col);
}

void AGridCellActor::InsertUnit(AGridUnitActor* Unit)
{
	UnitActors.Add(Unit);

	FGameplayTag unitStorage = FGameplayTag::RequestGameplayTag("Unit.Type.Storage");
	FGameplayTag unitLand = FGameplayTag::RequestGameplayTag("Unit.Type.Land");
	FGameplayTag unitBuilding = FGameplayTag::RequestGameplayTag("Unit.Type.Building");
	UnitActors.Sort([&](const AGridUnitActor& lhs, const AGridUnitActor& rhs)
	{
		int32 prioLeft = lhs.UnitTags.HasTagExact(unitStorage) ? 0 :
			lhs.UnitTags.HasTagExact(unitLand) ? 1 :
			lhs.UnitTags.HasTagExact(unitBuilding) ? 2 : 3;

		int32 prioRight = rhs.UnitTags.HasTagExact(unitStorage) ? 0 :
			rhs.UnitTags.HasTagExact(unitLand) ? 1 :
			rhs.UnitTags.HasTagExact(unitBuilding) ? 2 : 3;
		
		check(prioLeft != prioRight);
		return prioLeft < prioRight;
	});
}

void AGridCellActor::RemoveUnit(AGridUnitActor* Unit)
{
	UnitActors.Remove(Unit);
}

void AGridCellActor::RefreshLocation()
{
	for(auto& unit : UnitActors)
	{
		unit->RefreshLocation();
	}
}
