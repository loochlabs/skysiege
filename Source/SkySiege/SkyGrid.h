// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "Coordinates.h"
#include "GameFramework/Actor.h"
#include "SkyGrid.generated.h"

class AGridUnitActor;
class AGridCellActor;
struct FCoordinates;

UENUM(BlueprintType)
enum class EGridType : uint8
{
	Main = 0,
	Storage,
	Shop,
};


USTRUCT(BlueprintType)
struct FGridConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	TSubclassOf<class ASkyGrid> GridClass = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGridType Type = EGridType::Main;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rows = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cols = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CellPadding = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldLocation = FVector::ZeroVector;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator WorldRotation = FRotator::ZeroRotator;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridFocusedDelegate, AGridCellActor*, Cell);

UCLASS()
class SKYSIEGE_API ASkyGrid : public AActor
{
	GENERATED_BODY()
	
public:	

	void Setup(const FGridConfig& InGridConfig);
	void Teardown();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetupBP();

	//@OLD
	void SetViewToMacro();

	//@OLD
	UFUNCTION(BlueprintImplementableEvent)
	void SetViewToMacroBP();
	
	UFUNCTION(BlueprintCallable)
	void CreateCell(int32 row, int32 col);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateCellBP(AGridCellActor* cell);

	UFUNCTION(BlueprintCallable)
	void Cleanup();

	UFUNCTION(BlueprintImplementableEvent)
	void CleanupBP();
	
	UFUNCTION(BlueprintCallable)
	AGridCellActor* GetCell(int32 row, int32 col);

	void GetCellShape(TArray<AGridCellActor*>& Out, int32 Row, int32 Col, const TArray<FCoordinates>& Shape);
	AGridCellActor* FindValidCell(AGridUnitActor* Unit);
	
	UFUNCTION(BlueprintCallable)
	bool IsGridUnitEmpty(int32 row, int32 col);

	UFUNCTION(BlueprintCallable)
	bool GetRandomEmptyCell(int32& row, int32& col);

	void SetFocus(int32 Row, int32 Col);
	void CycleFocus();
	void Interact(int32 Row, int32 Col);
	void ClearAllHighlights();
	void RefreshHighlights();

	bool PlaceUnit(AGridUnitActor* Unit, int32 Row, int32 Col);
	void RefreshUnitBonuses();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGridType Type = EGridType::Main;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AGridCellActor> CellClass; 
	
	UPROPERTY(BlueprintReadWrite)
	TMap<FCoordinates, AGridCellActor*> Cells;

	FCoordinates Focus;
	int32 FocusIndex = 0;

	UPROPERTY(BlueprintAssignable)
	FGridFocusedDelegate OnFocused;

	UPROPERTY(BlueprintAssignable)
	FGridFocusedDelegate OnInteract;
};
