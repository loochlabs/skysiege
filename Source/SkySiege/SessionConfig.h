// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "SessionConfig.generated.h"


struct FGameplayTag;
struct FUnitTemplate;
class ASkyGrid;


USTRUCT(BlueprintType)
struct FSessionConfig
{
	GENERATED_BODY()

	void Setup();

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
	int32 ShopRerollCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules")
	TArray<FGameplayTag> ShopPoolTags;
};

