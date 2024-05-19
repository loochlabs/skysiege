// Property of Looch Labs LLC.


#include "SkyHUD.h"
#include "OverlayWidget.h"


void ASkyHUD::AddWidgetToViewport(UUserWidget* widget, EUIZOrderLayer layer /*= EUIZOrderLayer::None*/)
{
	widget->AddToViewport(static_cast<int32>(layer));
}

void ASkyHUD::Open(UStackWidget* widget)
{
	if(WindowStack.Num() > 0)
		WindowStack.Last()->Hide();

	WindowStack.Add(widget);
	widget->Open(this);
}

void ASkyHUD::Back()
{
	if(WindowStack.Num() == 0) return;

	UStackWidget* widget = WindowStack.Last();
	WindowStack.RemoveAt(WindowStack.Num()-1);
	widget->Close();

	if(WindowStack.Num() > 0)
		WindowStack.Last()->Show();
}

void ASkyHUD::Cleanup()
{
	for(auto& widget : WindowStack)
		widget->Close();

	WindowStack.Empty();
}

void ASkyHUD::ShowMainMenu()
{
	if(IsValid(MenuWidget))
	{
		MenuWidget->RemoveFromParent();
	}

	MenuWidget = CreateWidget<UStackWidget>(GetOwningPlayerController(), MainMenuWidgetClass);
	AddWidgetToViewport(MenuWidget);
	Open(MenuWidget);
}

void ASkyHUD::ShowGameOverlay()
{
	if(IsValid(OverlayWidget))
	{
		OverlayWidget->RemoveFromParent();
	}
	
	OverlayWidget = CreateWidget<UOverlayWidget>(GetOwningPlayerController(), OverlayWidgetClass);
	AddWidgetToViewport(OverlayWidget);
	Open(OverlayWidget);
}

void ASkyHUD::ShowBattle(FBattleSimulation& Sim)
{
	OverlayWidget->ShowBattle(Sim);
}
