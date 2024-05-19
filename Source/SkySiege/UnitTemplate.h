// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnitStats.h"
#include "UObject/NoExportTypes.h"
#include "UnitTemplate.generated.h"


struct FCoordinates;
class AGridUnitActor;


USTRUCT(BlueprintType)
struct FUnitBonusConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FCoordinates> Coords;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer TagsRequiredToApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer BonusTags;
};


USTRUCT(BlueprintType)
struct FUnitTemplate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGridUnitActor> UnitClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	int32 ShopCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shop")
	float ShopWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	TArray<FCoordinates> GridShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer UnitTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer TagsRequiredToBuild;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags")
	FGameplayTagContainer TagsBlockingBuild;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	FUnitStats UnitStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	TArray<FUnitBonusConfig> BonusConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	FString TooltipName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	UTexture2D* Icon = nullptr;
};
