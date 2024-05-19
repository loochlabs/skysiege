// Property of Looch Labs LLC.


#include "MainMenuWidget.h"

#include "SkyGameModeBase.h"
#include "SkyHUD.h"


void UMainMenuWidget::StartNewGame()
{
	Back();
	ASkyGameMode::Get(this)->StartGame();
	HUD->ShowGameOverlay();
}
