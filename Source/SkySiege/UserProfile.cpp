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
	GridMain->Teardown();
	GridMain = nullptr;
	GridStorage->Teardown();
    GridStorage = nullptr;
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
			
		}
		
		break;

	case ESessionPhase::Battle:
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

void UUserProfile::AddUnitToInventory(const FName& UnitKey)
{
	int32 foundIdx = -1;
	for(int32 idx = 0; idx < InventoryUnits.Num(); ++idx)
	{
		if(InventoryUnits[idx].UnitKey == UnitKey)
		{
			check(InventoryUnits[idx].Count > 0);
			foundIdx = idx;
			break;
		}
	}
	
	if(foundIdx >= 0)
	{
		InventoryUnits[foundIdx].Count++;
	}
	else
	{
		FInventoryUnit invUnit;
		invUnit.UnitKey = UnitKey;
		invUnit.Count = 1;
		InventoryUnits.Add(invUnit);
	}
	
	OnUpdatedUnitInventory.Broadcast();
}

void UUserProfile::RemoveUnitFromInventory(const FName& UnitKey)
{
	for(int32 idx = 0; idx < InventoryUnits.Num(); ++idx)
	{
		if(InventoryUnits[idx].UnitKey == UnitKey)
		{
			check(InventoryUnits[idx].Count > 0);
			InventoryUnits[idx].Count--;

			if(InventoryUnits[idx].Count <= 0)
				InventoryUnits.RemoveAt(idx);

			OnUpdatedUnitInventory.Broadcast();
			return;
		}
	}
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
	AddUnitToInventory(CurrentShopOptions[OptionIndex].UnitKey);
	UpdateWallet(-CurrentShopOptions[OptionIndex].Cost);
}

bool UUserProfile::StartTransaction(int32 index)
{
	if(index < 0 || index >= InventoryUnits.Num())
	{
		return false;
	}

	ActiveTransaction.InvIndex = index;
	FName unitKey = GetTransactionUnitKey();
	ActiveTransaction.UnitActor = CreateUnit(unitKey);
	OnUpdatedTransaction.Broadcast();
	return true;
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

void UUserProfile::TransactionRotate(bool bRotateCW)
{
	check(IsTransactionActive());
	ActiveTransaction.UnitActor->Rotate();
	GridMain->RefreshHighlights();
	OnUpdatedTransaction.Broadcast();
}

void UUserProfile::EndTransaction()
{
	if(!IsTransactionActive())
		return;

	check(ActiveTransaction.InvIndex >= 0);
	ActiveTransaction.UnitActor->Destroy();
	ActiveTransaction.Reset();
	GridMain->RefreshHighlights();
	OnUpdatedTransaction.Broadcast();
}

bool UUserProfile::ConfirmTransaction(ASkyGrid* Grid, int32 Row, int32 Col)
{
	if(!IsTransactionActive())
		return false;
	
	if(!PlaceUnit(Grid, ActiveTransaction.UnitActor, Row, Col))
		return false;
	
	RemoveUnitFromInventory(ActiveTransaction.UnitActor->UnitKey);

	ActiveTransaction.Reset();
	GridMain->RefreshHighlights();

	OnUpdatedTransaction.Broadcast();
	return true;
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

void UUserProfile::ClearUnit(ASkyGrid* Grid, AGridUnitActor* Unit)
{
	if(!IsValid(Unit))
		return;

	Unit->Destroy();
	AddUnitToInventory(Unit->UnitKey);

	//remove buffs
	auto& unitTemplate = ASkyGameMode::Get(this)->GetUnitTemplate(Unit->UnitKey);
	for(auto& bonusCfg : unitTemplate.BonusConfig)
	{
		auto buffCoords = Unit->GetOrientedCoords(bonusCfg.Coords);
		TArray<AGridCellActor*> cells;
		Grid->GetCellShape(cells, Unit->OriginGridCell->Row, Unit->OriginGridCell->Col, buffCoords);
		for(auto& cell : cells)
		{
			for(auto& cellUnit : cell->UnitActors)
			{
				cellUnit->UnitTags.RemoveTags(bonusCfg.BonusTags);
			}
		}
	}

	auto coords = Unit->GetOrientedCoords(unitTemplate.GridShape);
	AGridCellActor* origin = Unit->OriginGridCell;
	TArray<AGridCellActor*> unitCells;
	Grid->GetCellShape(unitCells, origin->Row, origin->Col, coords);
	TArray<AGridUnitActor*> unitsToRemove;

	for(AGridCellActor* cell : unitCells)
	{
		cell->RemoveUnit(Unit);

		for(AGridUnitActor* unit : cell->UnitActors)
		{
			auto& cellUnitTemplate = ASkyGameMode::Get(this)->GetUnitTemplate(unit->UnitKey);
			auto& tags = cellUnitTemplate.TagsRequiredToBuild;
			if(!tags.IsEmpty() && !cell->HasUnitTags(tags))
				unitsToRemove.AddUnique(unit);
		}
	}
	
	for(AGridUnitActor* unit : unitsToRemove)
	{
		ClearUnit(Grid, unit);
	}
}

const FName& UUserProfile::GetUnitKeyFromInventory(int32 InventoryIndex)
{
	return InventoryUnits[InventoryIndex].UnitKey;
}

const FName& UUserProfile::GetTransactionUnitKey()
{
	check(IsTransactionActive());
	return GetUnitKeyFromInventory(ActiveTransaction.InvIndex);
}

const FUnitTemplate& UUserProfile::GetTransactionUnitTemplate()
{
	FName unitKey = GetTransactionUnitKey();
	return ASkyGameMode::Get(this)->GetUnitTemplate(unitKey);
}

bool UUserProfile::IsTransactionActive()
{
	return ActiveTransaction.InvIndex != -1;
}

void UUserProfile::HandleGridFocused(AGridCellActor* Cell)
{
	//get current transaction shape, set highlight for each cell in shape to our result
	if(IsTransactionActive())
	{
		ActiveTransaction.UnitActor->SetOriginCell(Cell);
		
		//check each cell for valid tags
		auto& unitBP = GetTransactionUnitTemplate();
		auto coords = ActiveTransaction.UnitActor->GetOrientedCoords(unitBP.GridShape);
		TArray<AGridCellActor*> cells;
		Cell->Grid->GetCellShape(cells, Cell->Row, Cell->Col, coords);
		
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
			
			FName unitKey = unit->UnitKey;
			ClearUnit(Cell->Grid, unit);

			//restart the transaction with the unit we just removed
			int32 inventoryIdx = 0;
			for(int32 idx = 0; idx < InventoryUnits.Num(); ++idx)
			{
				if(InventoryUnits[idx].UnitKey == unitKey)
				{
					inventoryIdx = idx;
					break;
				}
			}
			StartTransaction(inventoryIdx);

			Cell->Grid->RefreshUnitBonuses();
			Cell->RefreshHighlight();
		}
	}
}