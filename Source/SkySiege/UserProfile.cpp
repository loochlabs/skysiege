// Property of Looch Labs LLC.


#include "UserProfile.h"

#include "GameplayTagContainer.h"
#include "GridCellActor.h"
#include "GridUnitActor.h"
#include "SkyGameModeBase.h"
#include "SkyGrid.h"
#include "UnitTemplate.h"
#include "CellHighlights.h"


const FSessionConfig& UUserProfile::GetConfig()
{
	return ASkyGameMode::Get(this)->Config;	
}

void UUserProfile::Setup(const FUserProfileConfig& InConfig)
{
	CurrentLevel = -1;
	
	//spawn grid
	check(!IsValid(GridMain));
	UWorld* world = GetWorld();
	GridMain = world->SpawnActor<ASkyGrid>(InConfig.GridConfig.GridClass);
	GridMain->Setup(InConfig.GridConfig);
	GridMain->OnFocused.AddDynamic(this, &UUserProfile::HandleGridFocused);
	GridMain->OnInteract.AddDynamic(this, &UUserProfile::HandleGridInteract);

	if(InConfig.bSpawnStorage)
	{
		GridStorage = world->SpawnActor<ASkyGrid>(InConfig.GridStorageConfig.GridClass);
		GridStorage->Setup(InConfig.GridStorageConfig);
		GridStorage->OnFocused.AddDynamic(this, &UUserProfile::HandleGridFocused);
		GridStorage->OnInteract.AddDynamic(this, &UUserProfile::HandleGridInteract);

		// add storage units to all cells
		for(auto& cellPair : GridStorage->Cells)
		{
			int32 row = cellPair.Key.Row;
			int32 col = cellPair.Key.Col;
			PlaceUnit(GridStorage, "unit_storage", row, col);
		}
	}

	// starting units
	for(const FUnitBuildTemplate& unit : InConfig.StartingUnits)
	{
		PlaceUnit(GridMain, unit.UnitKey, unit.Row, unit.Col);
	}
}

void UUserProfile::Teardown()
{
	if(IsValid(GridMain))
	{
		GridMain->Teardown();
		GridMain = nullptr;	
	}

	if(IsValid(GridStorage))
	{
		GridStorage->Teardown();
		GridStorage = nullptr;
	}
}

void UUserProfile::StartPhase(ESessionPhase Phase)
{
	switch(Phase)
	{
	case ESessionPhase::Shop:
		{
			CurrentLevel++;
			auto& cfg = GetConfig();
			UpdateWallet(cfg.StartingWalletAmount);
			ResetShopOptions();

			GridStorage->ResetLocationAndRotation();

			// process upgrade tags
			GridMain->UpgradeUnits();
		}
		
		break;

	case ESessionPhase::Battle:
		GridStorage->MoveOffscreen();
		break;

	default:
		break;
	}
}

void UUserProfile::UpdateWallet(int32 Amount)
{
	Wallet += Amount;
	OnUpdatedWallet.Broadcast();
}

void UUserProfile::ResetShopOptions()
{
	// accumulate all pool tags up to CurrentLevel
	auto& cfg = GetConfig();
	FGameplayTagContainer shopPoolTags;
	for(int32 lvl = 0; lvl <= FMath::Min(CurrentLevel, cfg.ShopPoolTags.Num()-1); ++lvl)
	{
		FGameplayTag tag = cfg.ShopPoolTags[lvl];
		shopPoolTags.AddTag(tag);
	}

	//get current shop pool
	struct ShopPoolItem
	{
		FName Key;
		float Weight = 1.f;
		int32 Cost = 0;
	};
	TArray<ShopPoolItem> shopPool;
	
	float weightTotal = 0.f;
	for(auto& unitPair : cfg.UnitTemplates)
	{
		if(unitPair.Value.UnitTags.HasAny(shopPoolTags))
		{
			ShopPoolItem item;
			item.Key = unitPair.Key;
			item.Weight = unitPair.Value.ShopWeight;
			item.Cost = unitPair.Value.ShopCost;
			shopPool.Add(item);
			weightTotal += item.Weight;
		}
	}

	// sum and normalize weights
	float weightAcum = 0.f;
	for(auto& item : shopPool)
	{
		item.Weight /= weightTotal;
		weightAcum += item.Weight;	
		item.Weight = weightAcum;
	}

	// roll for shop options
	CurrentShopOptions.Reset(); //@TODO temp until we can reserve slots

	// the last element should be weight=1
	for(int32 n = 0; n < NumOfShopOptions; ++n)
	{
		float rng = FMath::FRand();
		for(auto& item : shopPool)
		{
			if(rng < item.Weight)
			{
				FShopOption option;
				option.UnitKey = item.Key;
				option.Cost = item.Cost; 
				CurrentShopOptions.Add(option);
				break;
			}
		}
		// saftety add incase our math sucks
		if(CurrentShopOptions.Num()-1 != n)
		{
			FShopOption option;
			option.UnitKey = shopPool[0].Key;
			CurrentShopOptions.Add(option);
		}
	}
	check(CurrentShopOptions.Num() == NumOfShopOptions);

	OnUpdatedShop.Broadcast();
}


void UUserProfile::RerollShop()
{
	auto& cfg = GetConfig();
	UpdateWallet(-cfg.ShopRerollCost);
	ResetShopOptions();
}

bool UUserProfile::CanRerollShop()
{
	return Wallet >= GetConfig().ShopRerollCost;
}

bool UUserProfile::CanAfford(int32 OptionIndex)
{
	return Wallet >= CurrentShopOptions[OptionIndex].Cost;
}

void UUserProfile::ConfirmShopPurchase(int32 OptionIndex)
{
	check(CurrentShopOptions.Num() > 0);
	check(OptionIndex >= 0 && OptionIndex < CurrentShopOptions.Num());
	check(!CurrentShopOptions[OptionIndex].Purchased);
	
	CurrentShopOptions[OptionIndex].Purchased = true;
	UpdateWallet(-CurrentShopOptions[OptionIndex].Cost);

	AGridUnitActor* unit = CreateUnit(CurrentShopOptions[OptionIndex].UnitKey);
	AGridCellActor* cell = GridStorage->FindValidCell(unit);
	PlaceUnit(GridStorage, unit, cell->Row, cell->Col);
	StartTransaction(unit);
}

int32 UUserProfile::GetTransactionSellPrice()
{
	return IsTransactionActive() ? ActiveTransaction.UnitActor->GetSellPrice() : 0;
}

int32 UUserProfile::TryToSell()
{
	if(!IsTransactionActive())
		return 0;

	int32 sellPrice = GetTransactionSellPrice();
	UpdateWallet(sellPrice);
	ActiveTransaction.UnitActor->Teardown();
	ActiveTransaction.Reset();
	OnUpdatedTransaction.Broadcast();
	return sellPrice;
}

void UUserProfile::TryToCancel()
{
	if(IsTransactionActive())
	{
		CancelTransaction();
	}
}

void UUserProfile::TryToCycle(bool CW)
{
	if(IsTransactionActive())
	{
		TransactionRotate(CW);
	}
	else
	{
		GridMain->CycleFocus();
		GridStorage->CycleFocus();
	}
}

AGridUnitActor* UUserProfile::CreateUnit(const FName& InUnitKey)
{
	auto& unitTemplate = ASkyGameMode::Get(this)->GetUnitTemplate(InUnitKey);;
	FTransform transform;
	AGridUnitActor* unit = GetWorld()->SpawnActor<AGridUnitActor>(unitTemplate.UnitClass, transform);
	unit->Setup(InUnitKey);
	OnUnitCreated.Broadcast();
	return unit;
}

bool UUserProfile::PlaceUnit(ASkyGrid* Grid, const FName& UnitKey, int32 Row, int32 Col)
{
	AGridUnitActor* unitActor = CreateUnit(UnitKey);
	return PlaceUnit(Grid, unitActor, Row, Col);
}

bool UUserProfile::PlaceUnit(ASkyGrid* InGrid, AGridUnitActor* UnitActor, int32 Row, int32 Col)
{
	check(UnitActor);
	return InGrid->PlaceUnit(UnitActor, Row, Col);
}

void UUserProfile::ClearUnit(AGridUnitActor* Unit)
{
	//remove buffs
	ASkyGrid* grid = Unit->OriginGridCell->Grid;
	int32 row = Unit->OriginGridCell->Row;
	int32 col = Unit->OriginGridCell->Col;

	// remove this unit's bonuses
	auto& unitTemplate = ASkyGameMode::Get(this)->GetUnitTemplate(Unit->UnitKey);
	for(auto& bonusCfg : unitTemplate.BonusConfig)
	{
		auto buffCoords = Unit->GetOrientedCoords(bonusCfg.Coords);
		TArray<AGridCellActor*> buffCells;
		grid->GetCellShape(buffCells, row, col, buffCoords);
		for(auto& cell : buffCells)
		{
			for(auto& cellUnit : cell->UnitActors)
			{
				cellUnit->UnitTags.RemoveTags(bonusCfg.BonusTags);
			}
		}
	}

	// Remove this Unit, move any other dependent units to storage
	TArray<AGridCellActor*> unitCells;
	grid->GetCellShape(unitCells, row, col, Unit->GetOrientedShape());
	TArray<AGridUnitActor*> dependentUnits;

	for(AGridCellActor* cell : unitCells)
	{
		cell->RemoveUnit(Unit);

		for(AGridUnitActor* unit : cell->UnitActors)
		{
			auto& cellUnitTemplate = ASkyGameMode::Get(this)->GetUnitTemplate(unit->UnitKey);
			auto& tags = cellUnitTemplate.TagsRequiredToBuild;
			if(!tags.IsEmpty() && !cell->HasUnitTags(tags))
				dependentUnits.AddUnique(unit);
		}
	}

	// move any dependent units to storage
	for(AGridUnitActor* unit : dependentUnits)
	{
		MoveUnit(unit, GridStorage);
	}
}

// Move to storage, find an open cell for this unit, clear it's buffs/dependents, move the unit to it's ToGrid
void UUserProfile::MoveUnit(AGridUnitActor* Unit, ASkyGrid* DestGrid)
{
	ClearUnit(Unit);
	auto shape = Unit->GetOrientedShape();
	AGridCellActor* newCell = DestGrid->FindValidCell(Unit);
	check(newCell);
	DestGrid->PlaceUnit(Unit, newCell->Row, newCell->Col);
}

bool UUserProfile::IsTransactionActive()
{
	return ActiveTransaction.Active;
}

void UUserProfile::StartTransaction(AGridUnitActor* Unit)
{
	ActiveTransaction.Active = true;
	ActiveTransaction.UnitActor = Unit;
	ActiveTransaction.OriginalCell = Unit->OriginGridCell;
	ActiveTransaction.OriginalOrientation = Unit->GridOrientation;

	// lift this unit actor from it's cells, clearing up any dependencies
	ClearUnit(Unit);
	Unit->OriginGridCell->Grid->RefreshUnitBonuses();
	Unit->OriginGridCell->RefreshHighlight();
	OnUpdatedTransaction.Broadcast();
}

bool UUserProfile::ConfirmTransaction(ASkyGrid* Grid, int32 Row, int32 Col)
{
	if(!IsTransactionActive())
		return false;
	
	if(!Grid->PlaceUnit(ActiveTransaction.UnitActor, Row, Col))
		return false;

	ActiveTransaction.Reset();

	AGridCellActor* cell = Grid->GetCell(Row, Col);
	HandleGridFocused(cell);
	
	OnUpdatedTransaction.Broadcast();
	return true;
}

void UUserProfile::TransactionRotate(bool bRotateCW)
{
	check(IsTransactionActive());
	ActiveTransaction.UnitActor->Rotate();
	GridMain->RefreshHighlights();
	OnUpdatedTransaction.Broadcast();
}

void UUserProfile::CancelTransaction()
{
	check(IsTransactionActive());

	AGridUnitActor* unit = ActiveTransaction.UnitActor;
	ECellOrientation orientation = ActiveTransaction.OriginalOrientation;
	AGridCellActor* cell = ActiveTransaction.OriginalCell;
	ActiveTransaction.Reset();

	// reset UnitActor back to it's original locaiton, orientation
	unit->RotateTo(orientation);
	cell->Grid->PlaceUnit(unit, cell->Row, cell->Col);

	GridMain->RefreshHighlights();
	GridMain->RefreshUnitBonuses();
	GridStorage->RefreshHighlights();
	GridStorage->RefreshUnitBonuses();
	
	OnUpdatedTransaction.Broadcast();
}

void UUserProfile::HandleGridFocused(AGridCellActor* Cell)
{
	Cell->Grid->CycleFocusToTop();

	//get current transaction shape, set highlight for each cell in shape to our result
	if(IsTransactionActive())
	{
		ActiveTransaction.UnitActor->SetOriginCell(Cell);
		auto coords = ActiveTransaction.UnitActor->GetOrientedShape();
		TArray<AGridCellActor*> cells;
		Cell->Grid->GetCellShape(cells, Cell->Row, Cell->Col, coords);
		auto& unitBP = ASkyGameMode::Get(this)->GetUnitTemplate(ActiveTransaction.UnitActor->UnitKey);
		
		for(auto& c : cells)
		{
			bool bValidTransaction = c->CanBuild(unitBP);
			c->SetHighlight(bValidTransaction ? ECellHighlight::Build : ECellHighlight::Invalid);
		}
	}
	else
	{
		Cell->SetHighlight(ECellHighlight::Basic);
	}
}

void UUserProfile::HandleGridInteract(AGridCellActor* Cell)
{
	if(IsTransactionActive())
	{
		ConfirmTransaction(Cell->Grid, Cell->Row, Cell->Col);
	}
	else
	{
		// clear units
		if(Cell->UnitActors.Num() > 0)
		{
			int32 focusIdx = Cell->Grid->FocusIndex % Cell->UnitActors.Num();
			AGridUnitActor* unit = Cell->UnitActors[focusIdx];

			// check if we can pickup this unit
			FGameplayTag tag = FGameplayTag::RequestGameplayTag("Unit.Type.Storage");
			FGameplayTagContainer tags;
			tags.AddTag(tag);
			if(unit->UnitTags.HasAny(tags))
			{
				return;
			}
			StartTransaction(unit);
		}
	}
}