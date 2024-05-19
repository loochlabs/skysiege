// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "SessionConfig.h"
#include "GameFramework/GameModeBase.h"
#include "SkyGameModeBase.generated.h"


UCLASS()
class SKYSIEGE_API ASkyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	const FUnitTemplate& GetUnitTemplate(const FName& UnitKey);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSessionConfig Config;
};
