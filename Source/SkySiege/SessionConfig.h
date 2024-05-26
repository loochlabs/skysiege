// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "SkyGrid.h"
#include "SessionConfig.generated.h"


struct FUnitTagUpgrade;
struct FGameplayTag;
struct FUnitTemplate;
class ASkyGrid;


USTRUCT(BlueprintType)
struct FSessionConfig
{
	GENERATED_BODY()

	void Setup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FGridConfig GridConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FGridConfig GridStorageConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FGridConfig GridShopConfig;

	UPROPERTY(EditAnywhere, Category="Unit Data")
	UDataTable* UnitTemplateData = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category="Unit Data")
	TMap<FName, FUnitTemplate> UnitTemplates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Unit Data")
	TArray<FUnitTagUpgrade> UnitUpgrades;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
	int32 WinsRequired = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
	int32 LosesAllowed = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
	int32 BattleStartingMaxHP = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
	int32 BattleStartingFood = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
	int32 BattleFoodGeneration = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle")
	int32 BattleFoodGenerationRate = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	int32 StartingWalletAmount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	int32 ShopRerollCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	int32 ShopOptionsCount = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	TArray<FGameplayTag> ShopPoolTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	TSubclassOf<AActor> ShopSellClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	FVector ShopSellLocation = FVector::ZeroVector;
};

