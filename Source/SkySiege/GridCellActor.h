// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCellActor.generated.h"

class ASkyGrid;
class AGridUnitActor;
struct FUnitTemplate;
struct FGameplayTagContainer;
enum class ECellHighlight : uint8;
enum class ECellOrientation : uint8;

UCLASS()
class SKYSIEGE_API AGridCellActor : public AActor
{
	GENERATED_BODY()
	
public:

	void Setup(ASkyGrid* ParentGrid, int32 InRow, int32 InCol);
	void Teardown();

	void SetSelectable(bool bEnable);
	FVector GetUnitAnchorLocation();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdatedSelectable();

	void BeginCleanup();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BeginCleanupBP();
	
	UFUNCTION(BlueprintCallable)
	void Cleanup();

	UFUNCTION(BlueprintImplementableEvent)
	void UnitCreatedBP(AGridUnitActor* Unit, ECellOrientation Orientation);

	bool CanBuild(const FUnitTemplate& UnitBP);
	bool HasUnitTags(const FGameplayTagContainer& InTags);
	
	UFUNCTION(BlueprintCallable)
	void BeginFocus();

	int32 GetUnitFocusIndex();
	void FocusUnit();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BeginHighlightBP(int32 FocusIndex);
	
	UFUNCTION(BlueprintCallable)
	void EndFocus();
	
	UFUNCTION(BlueprintImplementableEvent)
	void EndHighlightBP();
	
	UFUNCTION(BlueprintCallable)
	void RefreshHighlight();

	UFUNCTION(BlueprintCallable)
	void SetHighlight(ECellHighlight InHighlight);

	UFUNCTION(BlueprintCallable)
	void Interact();

	void InsertUnit(AGridUnitActor* Unit);
	void RemoveUnit(AGridUnitActor* Unit);

	UPROPERTY(BlueprintReadWrite)
	int32 Row = -1;
	
	UPROPERTY(BlueprintReadWrite)
	int32 Col = -1;

	UPROPERTY(BlueprintReadOnly)
	ASkyGrid* Grid;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<AGridUnitActor*> UnitActors;
	
	UPROPERTY(BlueprintReadWrite)
	ECellHighlight Highlight;

	UPROPERTY(BlueprintReadWrite)
	bool bSelectable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector UnitAnchorLocation = FVector::ZeroVector;
};
