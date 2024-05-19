// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StackWidget.h"
#include "SkyHUD.generated.h"


class UOverlayWidget;
struct FBattleSimulation;

// Global Z Order for all widgets in the game. Higher order layers are rendered on the top. (defined by UE)
// Always call Gi->AddWidgetToViewport() to ensure one of these global UI layers is assigned. 
UENUM(BlueprintType)
enum class EUIZOrderLayer : uint8
{
	None = 0,
	Game = 10,
	GameOverlay = 100,
	Menu = 200,
};


UCLASS()
class SKYSIEGE_API ASkyHUD : public AHUD
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void AddWidgetToViewport(UUserWidget* widget, EUIZOrderLayer layer = EUIZOrderLayer::None);
	
	UFUNCTION(BlueprintCallable)
	void Open(UStackWidget* widget);

	UFUNCTION(BlueprintCallable)
	void Back();

	UFUNCTION(BlueprintCallable)
	void Cleanup();

	void ShowMainMenu();
	void ShowGameOverlay();
	void ShowBattle(FBattleSimulation& Sim);

	UPROPERTY(BlueprintReadWrite)
	UStackWidget* MenuWidget = nullptr;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UStackWidget> MainMenuWidgetClass = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UOverlayWidget* OverlayWidget = nullptr;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidget> OverlayWidgetClass = nullptr;
	
	UPROPERTY()
	TArray<UStackWidget*> WindowStack;
};

