// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "StackWidget.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class SKYSIEGE_API UMainMenuWidget : public UStackWidget
{
	GENERATED_BODY()

public:
    
	UFUNCTION(BlueprintCallable)
	void StartNewGame();
};
