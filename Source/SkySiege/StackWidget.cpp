// Property of Looch Labs LLC.


#include "StackWidget.h"
#include "SkyHUD.h"

void UStackWidget::Back()
{
	HUD->Back();	
}

void UStackWidget::Open(ASkyHUD* InHUD)
{
	HUD = InHUD;
	OpenBP();
}

void UStackWidget::Show()
{
	ShowBP();
}

void UStackWidget::Hide()
{
	HideBP();
}

void UStackWidget::Close()
{
	CloseBP();
	RemoveFromParent();
}
