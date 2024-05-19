// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnitStats.generated.h"


USTRUCT(BlueprintType)
struct FUnitStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Power = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Cost = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Cooldown = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Competence = 1.f; 	// chance to succeed
};

