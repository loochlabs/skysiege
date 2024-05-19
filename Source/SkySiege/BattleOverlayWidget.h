// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "BattleSim.h"
#include "Blueprint/UserWidget.h"
#include "BattleOverlayWidget.generated.h"

UCLASS()
class SKYSIEGE_API UBattleOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ShowSim(const FBattleSimulation& InSim);

	void DisplayFrame(int32 FrameTime);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDisplayFrame(const FBattleFrame& Frame);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FramesProcessedPerTick = 1;
	
	bool bShowSim = false;
	FBattleSimulation Sim;
	int32 SimFrame = 0;
};
