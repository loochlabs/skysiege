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
	check(!IsValid(Grid));
	UWorld* world = GetWorld();
	auto& cfg = GetConfig();
	Grid = world->SpawnActor<ASkyGrid>(cfg.GridClass);
	Grid->SetActorLocation(InConfig.GridLocation);
	Grid->SetActorRotation(InConfig.GridRotation);
	Grid->Cleanup();
	Grid->OnFocused.AddDynamic(this, &UUserProfile::HandleGridFocused);

	// hard coding grid spawning
	for(int32 c = 0; c < InConfig.GridCols; ++c)
	{
		for(int32 r = 0; r < InConfig.GridRows; ++r)
		{
			Grid->CreateCell(r, c);
		}
	}

	// padding
	for(int32 c = -InConfig.GridPadding; c < InConfig.GridPadding + InConfig.GridCols; ++c)
	{
		for(int32 r = -InConfig.GridPadding; r < InConfig.GridPadding + InConfig.GridRows; ++r)
		{
			if(Grid->GetCell(r, c))
				continue;

			Grid->CreateCell(r, c);
			AGridCellActor* cell = Grid->GetCell(r, c);
			cell->SetSelectable(false);
		}
	}

	// starting units
	for(const FUnitBuildTemplate& unit : InConfig.StartingUnits)
	{
		PlaceUnit(unit.UnitKey, unit.Row, unit.Col);
	}
}

void UUserProfile::Teardown()
{
	Grid->Teardown();
	Grid = nullptr;
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
	static TArray<FName> shopPoolKeys =
	{
		"Shop.Pool.Level_0",
		"Shop.Pool.Level_1",
		"Shop.Pool.Level_2",
	};
	FGameplayTagContainer shopPoolTags;
	for(int32 lvl = 0; lvl <= FMath::Min(CurrentLevel, shopPoolKeys.Num()-1); ++lvl)
	{
		FName poolTag = shopPoolKeys[lvl];
		FGameplayTag tag = FGameplayTag::RequestGameplayTag(poolTag);
		shopPoolTags.AddTag(tag);
	}

	//get current shop pool
	struct ShopPoolItem
	{
		FName Key;
		float Weight = 1.f;
	};
	TArray<ShopPoolItem> shopPool;
	
	float weightTotal = 0.f;
	for(auto& unitPair : GetConfig().UnitTemplates)
	{
		if(unitPair.Value.UnitTags.HasAny(shopPoolTags))
		{
			ShopPoolItem item;
			item.Key = unitPair.Key;
			item.Weight = unitPair.Value.ShopWeight;
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
	FName unitKey = CurrentShopOptions[OptionIndex].UnitKey;
	const FUnitTemplate& unitTemplate = ASkyGameMode::Get(this)->GetUnitTemplate(unitKey);
	return Wallet >= unitTemplate.ShopCost;
}

void UUserProfile::ConfirmShopPurchase(int32 OptionIndex)
{
	check(CurrentShopOptions.Num() > 0);
	check(OptionIndex >= 0 && OptionIndex < CurrentShopOptions.Num());
	check(!CurrentShopOptions[OptionIndex].Purchased);
	CurrentShopOptions[OptionIndex].Purchased = true;
	AddUnitToInventory(CurrentShopOptions[OptionIndex].UnitKey);

	FName unitKey = CurrentShopOptions[OptionIndex].UnitKey;
	const FUnitTemplate& unitTemplate = ASkyGameMode::Get(this)->GetUnitTemplate(unitKey);
	UpdateWallet(-unitTemplate.ShopCost);
}

void UUserProfile::TryToInteract(int32 Row, int32 Col)
{
	if(IsTransactionActive())
	{
		ConfirmTransaction(Row, Col);
	}
	else
	{
		// clear units
		AGridCellActor* cell = Grid->GetCell(Row, Col);
		if(cell->UnitActors.Num() > 0)
		{
			AGridUnitActor* unit = cell->UnitActors[CellUnitSelection];
			FName unitKey = unit->UnitKey;
			ClearUnit(unit);

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

			Grid->RefreshUnitBonuses();
			cell->RefreshHighlight();
		}
	}
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

void UUserProfile::TransactionRotate(bool bRotateCW)
{
	if(!IsTransactionActive()) return;
	
	ActiveTransaction.UnitActor->Rotate();
	Grid->RefreshHighlights();
	OnUpdatedTransaction.Broadcast();
}

void UUserProfile::EndTransaction()
{
	if(!IsTransactionActive())
		return;

	check(ActiveTransaction.InvIndex >= 0);
	ActiveTransaction.UnitActor->Destroy();
	ActiveTransaction.Reset();
	Grid->RefreshHighlights();
	OnUpdatedTransaction.Broadcast();
}

bool UUserProfile::ConfirmTransaction(int32 Row, int32 Col)
{
	if(!IsTransactionActive())
		return false;
	
	if(!PlaceUnit(ActiveTransaction.UnitActor, Row, Col))
		return false;

	
	RemoveUnitFromInventory(ActiveTransaction.UnitActor->UnitKey);

	ActiveTransaction.Reset();
	Grid->RefreshHighlights();

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

bool UUserProfile::PlaceUnit(const FName& UnitKey, int32 Row, int32 Col)
{
	AGridUnitActor* unitActor = CreateUnit(UnitKey);
	return PlaceUnit(unitActor, Row, Col);
}

bool UUserProfile::PlaceUnit(AGridUnitActor* UnitActor, int32 Row, int32 Col)
{
	check(UnitActor);
	return Grid->PlaceUnit(UnitActor, Row, Col);
}

void UUserProfile::ClearUnit(AGridUnitActor* Unit)
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
		ClearUnit(unit);
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
//void UUserProfile::BeginCellHighlight(int32 Row, int32 Col) @CLEAN
{
	//Grid->ClearAllHighlights();
	//Grid->SetFocus(Row, Col);
	//AGridCellActor* cell = Grid->GetCell(Row, Col);
	
	//get current transaction shape
	//set highlight for each cell in shape to our result
	if(IsTransactionActive())
	{
		ActiveTransaction.UnitActor->SetOriginCell(Cell);
		
		//check each cell for valid tags
		auto& unitBP = GetTransactionUnitTemplate();
		auto coords = ActiveTransaction.UnitActor->GetOrientedCoords(unitBP.GridShape);
		TArray<AGridCellActor*> cells;
		Grid->GetCellShape(cells, Cell->Row, Cell->Col, coords);
		
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
