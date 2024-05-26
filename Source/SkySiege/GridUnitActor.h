// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "CellOrientation.h"
#include "GameplayTagContainer.h"
#include "UnitStats.h"
#include "UnitTagUpgrade.h"
#include "GameFramework/Actor.h"
#include "GridUnitActor.generated.h"


struct FBattleEvent;
struct FUnitTemplate;
struct FCoordinates;
class AGridCellActor;


UCLASS()
class SKYSIEGE_API AGridUnitActor : public AActor
{
	GENERATED_BODY()

public:

	virtual void Setup(const FName& InUnitKey);
	void Teardown();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetupComplete();
	
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintPure)
	const FUnitTemplate& GetTemplate();

	UFUNCTION(BlueprintPure)
	const FUnitStats& GetStats();

	UFUNCTION(BlueprintPure)
	int32 GetSellPrice();

	void RefreshLocation();
	void SetOriginCell(AGridCellActor* Cell);
	void Rotate(bool CW = true);
	void RotateTo(ECellOrientation InOrientation);
	TArray<FCoordinates> GetOrientedCoords(const TArray<FCoordinates>& Coords);
	TArray<FCoordinates> GetOrientedShape();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdatedOrientation();

	UFUNCTION(BlueprintCallable)
	void SetFocus(bool bFocused);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdatedFocus(bool bFocused);
	
	void HandleBattleEvent(const FBattleEvent& Event);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBattleEvent(const FBattleEvent& Event);

	void RefreshTagUpgrades();
	void UpgradeTags();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdatedTags();
	
	void RefreshTags();
	
	UFUNCTION(BlueprintPure)
	FText GetTagTooltip(const FGameplayTag& Tag);
	
	UPROPERTY(BlueprintReadWrite)
	FName UnitKey;
	
	UPROPERTY(BlueprintReadWrite)
	int32 UnitID = -1;

	UPROPERTY(BlueprintReadWrite)
	AGridCellActor* OriginGridCell = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer UnitTags = FGameplayTagContainer();

	UPROPERTY(BlueprintReadWrite)
	ECellOrientation GridOrientation = ECellOrientation::North;

	UPROPERTY(BlueprintReadWrite)
	TArray<FUnitTagUpgrade> PendingUpgrades;
};

