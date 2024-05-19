// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StackWidget.generated.h"

class ASkyHUD;

UCLASS()
class SKYSIEGE_API UStackWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Back();
	void Open(ASkyHUD* InHUD);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OpenBP();
	
	void Show();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowBP();
	
	void Hide();
	
	UFUNCTION(BlueprintImplementableEvent)
	void HideBP();
	
	void Close();
	
	UFUNCTION(BlueprintImplementableEvent)
	void CloseBP();

	UPROPERTY()
	ASkyHUD* HUD;
};
