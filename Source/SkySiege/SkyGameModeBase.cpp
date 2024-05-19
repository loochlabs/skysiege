// Property of Looch Labs LLC.


#include "SkyGameModeBase.h"

#include "UnitTemplate.h"

void ASkyGameMode::BeginPlay()
{
	Super::BeginPlay();

	//@TODO initial setup of unit templates
}

const FUnitTemplate& ASkyGameMode::GetUnitTemplate(const FName& UnitKey)
{
	static FUnitTemplate temp;
	return temp;
}