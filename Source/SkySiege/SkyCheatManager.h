// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "SkyCheatManager.generated.h"


UCLASS()
class SKYSIEGE_API USkyCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	UFUNCTION(exec)
	void AddMoney(int32 Value);

	UFUNCTION(exec)
	void SetLevel(int32 Level);
};

