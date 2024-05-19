// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "StackWidget.h"
#include "OverlayWidget.generated.h"

struct FBattleSimulation;

UCLASS()
class SKYSIEGE_API UOverlayWidget : public UStackWidget
{
	GENERATED_BODY()

public:

	void ShowBattle(FBattleSimulation& Sim);

	UFUNCTION(BlueprintImplementableEvent)
	void OnShowBattle(const FBattleSimulation& Sim);
};
