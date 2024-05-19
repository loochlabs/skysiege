// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverlayWidget.generated.h"

struct FBattleSimulation;

UCLASS()
class SKYSIEGE_API UOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void ShowBattle(FBattleSimulation& Sim);

	UFUNCTION(BlueprintImplementableEvent)
	void OnShowBattle(const FBattleSimulation& Sim);
};
