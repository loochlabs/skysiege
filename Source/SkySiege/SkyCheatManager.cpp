// Property of Looch Labs LLC.


#include "SkyCheatManager.h"

#include "SkyGameModeBase.h"
#include "UserProfile.h"

void USkyCheatManager::AddMoney(int32 Value)
{
	ASkyGameMode* gamemode = ASkyGameMode::Get(this);
	gamemode->UserProfile->UpdateWallet(Value);
}

void USkyCheatManager::SetLevel(int32 Level)
{
	ASkyGameMode* gamemode = ASkyGameMode::Get(this);
	gamemode->UserProfile->CurrentLevel = Level;
}
