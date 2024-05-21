// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "CellOrientation.h"
#include "SkyGrid.h"
#include "UObject/NoExportTypes.h"
#include "UserProfile.generated.h"


class AGridCellActor;
struct FSessionConfig;
class ASkyGrid;
class AGridUnitActor;
struct FUnitTemplate;
struct FShopPool;
enum class ESessionPhase : uint8;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProfileUpdatedShopDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProfileUpdatedWalletDelegate);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProfileUpdatedUnitInventoryDelegate); @CLEAN
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProfileTransactionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProfileUnitCreatedDelegate);


USTRUCT(BlueprintType)
struct FShopOption
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FName UnitKey;

	UPROPERTY(BlueprintReadWrite)
	int32 Cost = 0;

	UPROPERTY(BlueprintReadWrite)
	bool Purchased = false;
};

struct FUnitBuildTemplate
{
	FName UnitKey;
	int32 Row;
	int32 Col;
};

USTRUCT()
struct FUserProfileConfig
{
	GENERATED_BODY()

	void AddStartingUnit(const FName& Key, int32 Row, int32 Col)
	{
		FUnitBuildTemplate unit;
		unit.UnitKey = Key;
		unit.Row = Row;
		unit.Col = Col;
		StartingUnits.Add(unit);
	}

	FGridConfig GridConfig;
	FGridConfig GridStorageConfig;
	
	bool bSpawnStorage = false;
	
	TArray<FUnitBuildTemplate> StartingUnits;	
};


//@CLEAN
// USTRUCT(BlueprintType)
// struct FInventoryUnit
// {
// 	GENERATED_BODY()
// 	
// 	UPROPERTY(BlueprintReadWrite)
// 	FName UnitKey;
//
// 	UPROPERTY(BlueprintReadWrite)
// 	int32 Count = 0;
// };

enum class ETransactionState : uint8
{
	None = 0,
	Active,
	Success
};

USTRUCT()
struct FTransactionData
{
	GENERATED_BODY()

	void Reset()
	{
		//InvIndex = -1; @CLEAN
		Active = false;
		UnitActor = nullptr;
	};

	//int32 InvIndex = -1;
	bool Active = false;
	AGridUnitActor* UnitActor = nullptr;
	AGridCellActor* OriginalCell = nullptr;
	ECellOrientation OriginalOrientation = ECellOrientation::North;
};


UCLASS()
class SKYSIEGE_API UUserProfile : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure)
	const FSessionConfig& GetConfig();
	
	void Setup(const FUserProfileConfig& InConfig);
	void Teardown();
	void StartPhase(ESessionPhase Phase);
	
	UFUNCTION(BlueprintCallable)
	void UpdateWallet(int32 Amount);

	void ResetShopOptions();

	UFUNCTION(BlueprintCallable)
	void RerollShop();

	UFUNCTION(BlueprintPure)
	bool CanRerollShop();

	UFUNCTION(BlueprintPure)
	bool CanAfford(int32 OptionIndex);
	
	UFUNCTION(BlueprintCallable)
	void ConfirmShopPurchase(int32 OptionIndex);

	UFUNCTION(BlueprintCallable)
	void TryToCancel();
	
	UFUNCTION(BlueprintCallable)
	void TryToCycle(bool CW);
	
	AGridUnitActor* CreateUnit(const FName& InUnitKey);
	bool PlaceUnit(ASkyGrid* Grid, const FName& UnitKey, int32 Row, int32 Col);
	bool PlaceUnit(ASkyGrid* Grid, AGridUnitActor* UnitActor, int32 Row, int32 Col);
	void ClearUnit(AGridUnitActor* Unit);
	void MoveUnit(AGridUnitActor* Unit, ASkyGrid* DestGrid);

	bool IsTransactionActive();
	void StartTransaction(AGridUnitActor* Unit);
	bool ConfirmTransaction(ASkyGrid* Grid, int32 Row, int32 Col);
	void TransactionRotate(bool bRotateCW);
	void CancelTransaction();
	
	UFUNCTION()
	void HandleGridFocused(AGridCellActor* Cell);

	UFUNCTION()
	void HandleGridInteract(AGridCellActor* Cell);
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentWins = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentLoses = 0;
	
	UPROPERTY()
	ASkyGrid* GridMain = nullptr;

	UPROPERTY()
	ASkyGrid* GridStorage = nullptr;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 Wallet;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentLevel = -1;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 NumOfShopOptions = 6;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	TArray<FShopOption> CurrentShopOptions;

	FTransactionData ActiveTransaction;

	UPROPERTY(BlueprintAssignable)
	FProfileUpdatedShopDelegate OnUpdatedShop;
	
	UPROPERTY(BlueprintAssignable)
	FProfileUpdatedWalletDelegate OnUpdatedWallet;

	UPROPERTY(BlueprintAssignable)
	FProfileTransactionDelegate OnUpdatedTransaction;

	UPROPERTY(BlueprintAssignable)
	FProfileUnitCreatedDelegate OnUnitCreated;
};
