// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "UnitTemplate.h"
#include "Engine/DataTable.h"
#include "UnitDataTableRow.generated.h"

USTRUCT()
struct SKYSIEGE_API FUnitDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FUnitTemplate UnitTemplate;
};
