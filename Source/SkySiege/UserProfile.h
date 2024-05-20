// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProfileUpdatedUnitInventoryDelegate);
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
	
	FVector GridLocation = FVector::ZeroVector;
	FRotator GridRotation = FRotator::ZeroRotator;
	int32 GridRows = 6;
	int32 GridCols = 8;
	int32 GridPadding = 0;
	TArray<FUnitBuildTemplate> StartingUnits;	
};


USTRUCT(BlueprintType)
struct FInventoryUnit
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FName UnitKey;

	UPROPERTY(BlueprintReadWrite)
	int32 Count = 0;
};

enum class ETransactionState : uint8
{
	None = 0,
	Active,
	Success
};

USTRUCT(BlueprintType)
struct FTransactionData
{
	GENERATED_BODY()

	void Reset()
	{
		InvIndex = -1;
		UnitActor = nullptr;
	};

	UPROPERTY(BlueprintReadWrite)
	int32 InvIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	AGridUnitActor* UnitActor = nullptr;
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

	UFUNCTION(BlueprintCallable)
	void AddUnitToInventory(const FName& UnitKey);

	UFUNCTION(BlueprintCallable)
	void RemoveUnitFromInventory(const FName& UnitKey);

	UFUNCTION(BlueprintPure)
	bool CanAfford(int32 OptionIndex);
	
	UFUNCTION(BlueprintCallable)
	void ConfirmShopPurchase(int32 OptionIndex);

	void TryToInteract(int32 Row, int32 Col);
	
	UFUNCTION(BlueprintCallable)
	bool StartTransaction(int32 index);
	
	UFUNCTION(BlueprintCallable)
	void TransactionRotate(bool bRotateCW);

	bool ConfirmTransaction(int32 Row, int32 Col);

	UFUNCTION(BlueprintCallable)
	void EndTransaction();
	
	AGridUnitActor* CreateUnit(const FName& InUnitKey);
	bool PlaceUnit(const FName& UnitKey, int32 Row, int32 Col);
	bool PlaceUnit(AGridUnitActor* UnitActor, int32 Row, int32 Col);
	void ClearUnit(AGridUnitActor* Unit);
	
	const FName& GetUnitKeyFromInventory(int32 InventoryIndex);
	const FName& GetTransactionUnitKey();
	const FUnitTemplate& GetTransactionUnitTemplate();

	UFUNCTION(BlueprintPure)
	bool IsTransactionActive();

	UFUNCTION()
	void HandleGridFocused(AGridCellActor* Cell);

	UFUNCTION()
	void HandleGridInteract(AGridCellActor* Cell);
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentWins = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentLoses = 0;
	
	UPROPERTY()
	ASkyGrid* Grid = nullptr;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 Wallet;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentLevel = -1;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 NumOfShopOptions = 6;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	TArray<FShopOption> CurrentShopOptions;

	UPROPERTY(BlueprintReadWrite)
	TArray<FInventoryUnit> InventoryUnits;

	UPROPERTY(BlueprintReadWrite)
	FTransactionData ActiveTransaction;

	int32 CellUnitSelection = 0;

	UPROPERTY(BlueprintAssignable)
	FProfileUpdatedShopDelegate OnUpdatedShop;
	
	UPROPERTY(BlueprintAssignable)
	FProfileUpdatedWalletDelegate OnUpdatedWallet;
	
	UPROPERTY(BlueprintAssignable)
	FProfileUpdatedUnitInventoryDelegate OnUpdatedUnitInventory;

	UPROPERTY(BlueprintAssignable)
	FProfileTransactionDelegate OnUpdatedTransaction;

	UPROPERTY(BlueprintAssignable)
	FProfileUnitCreatedDelegate OnUnitCreated;
};
