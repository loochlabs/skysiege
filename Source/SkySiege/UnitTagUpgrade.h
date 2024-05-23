// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "UnitTagUpgrade.generated.h"

USTRUCT(BlueprintType)
struct FUnitTagUpgrade
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer UpgradeTags;
};
