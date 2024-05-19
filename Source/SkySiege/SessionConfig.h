// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "SessionConfig.generated.h"



struct FUnitTemplate;
class ASkyGrid;


USTRUCT(BlueprintType)
struct FShopPool
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere)
	TArray<FName> UnitKeys;
};


USTRUCT(BlueprintType)
struct FSessionConfig
{
	GENERATED_BODY()

	void Setup(); //@TODO unit template setup

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	TSubclassOf<ASkyGrid> GridClass = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Unit Data")
	UDataTable* UnitTemplateData = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category="Unit Data")
	TMap<FName, FUnitTemplate> UnitTemplates;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rules")
	int32 WinsRequired = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rules")
	int32 LosesAllowed = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules")
	int32 StartingWalletAmount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules")
	TMap<FName, int32> StartingInventory;

	UPROPERTY(BlueprintReadWrite, Category="Rules")
	int32 ShopRerollCost = 1;

	UPROPERTY(BlueprintReadWrite, Category="Rules")
	TArray<FShopPool> ShopPools;
};

