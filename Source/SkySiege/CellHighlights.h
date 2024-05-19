// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECellHighlight : uint8
{
	None = 0,
	Basic,
	Build,
	Invalid
};
