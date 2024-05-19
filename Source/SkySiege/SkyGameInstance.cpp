// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyGameInstance.h"


#if UE_EDITOR + UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT != 0 
	const bool USkyGameInstance::DebugMode = true;
 #else
 	const bool USkyGameInstance::DebugMode = false;
#endif


void USkyGameInstance::Setup()
{
	// Global configs
	//MainLevelName = cfg->MainLevelName;

	//@TODO
	// Socials
	//SocialURL_Discord = cfg->SocialURL_Discord;
	//SocialURL_Twitter = cfg->SocialURL_Twitter;
	//SocialURL_Twitch = cfg->SocialURL_Twitch;
	//SocialURL_Steam = cfg->SocialURL_Steam;

	// Display text pulled from txt's
	FString versionPath = FPaths::ProjectContentDir();
	versionPath += FString(TEXT("App/version.txt"));
	FFileHelper::LoadFileToString(VersionLabel, *versionPath);

	// Display text pulled from txt's
	FString changelogPath = FPaths::ProjectContentDir();
	changelogPath += FString(TEXT("App/changelog.txt"));
	FFileHelper::LoadFileToString(ChangelogLabel, *changelogPath);

	

	//@TODO move this stuff to the GameMode
	// Session Rules. Create new rules for each difficulty settings
	// for(int32 diff = 0; diff < cfg->DifficultySettings.Num(); ++diff)
	// {
	// 	USessionRules* rules = NewObject<USessionRules>(this);
	// 	check(!SessionRules.Contains(diff));
	// 	SessionRules.Add(diff, rules);
	// 	
	// 	// Setup Normal Session Rules
	// 	rules->WinsRequired = cfg->WinsRequired;
	// 	rules->LosesAllowed = cfg->LosesAllowed;
	// 	rules->GridClass = cfg->GridClass;
	// 	rules->LevelSettings = cfg->DifficultySettings[diff].LevelSettings;
	// 	rules->StartingWalletAmount = cfg->StartingWalletAmount;
	// 	rules->StartingInventory = cfg->StartingInventory;
	//
	// 	// shops
	// 	rules->ShopRerollCost = cfg->ShopRerollCost;
	// 	//rules->ShopRerollCurrency = cfg->ShopRerollCurrency;
	// 	rules->ShopPools = cfg->ShopPools;
	// 	rules->BattleDisplayClass = cfg->BattleDisplayClass; //@CLEAN
	// 	
	// 	// setup Mine unit keys
	// 	rules->UnitTemplateBlueprints.Empty();
	// 	if(cfg->UnitTemplates)
	// 	{
	// 		for (auto& it : cfg->UnitTemplates->GetRowMap())
	// 		{
	// 			FUnitTemplateDataTable* row = (FUnitTemplateDataTable*)(it.Value);
	// 			check(!rules->UnitTemplateBlueprints.Contains(it.Key));
	// 			FUnitTemplateBlueprint unitBP;
	// 			unitBP.UnitKey = it.Key;
	// 			unitBP.TooltipName = row->TooltipName;
	// 			unitBP.ShopCost = row->ShopCost;
	// 			unitBP.ShopWeight = row->ShopWeight;
	// 			unitBP.BlueprintClass = row->UnitBlueprint;
	// 			unitBP.GridShape = row->GridShape;
	// 			unitBP.UnitTags = row->UnitTags;
	// 			unitBP.TagsRequiredToBuild = row->TagsRequiredToBuild;
	// 			unitBP.TagsBlockingBuild = row->TagsBlockingBuild;
	// 			unitBP.UnitStats = row->UnitStats;
	// 			unitBP.BonusConfig = row->BonusConfig;
	// 			unitBP.Icon = row->Icon;
	// 			rules->UnitTemplateBlueprints.Add(it.Key, unitBP);
	// 		}
	// 	}
	// }
}

void USkyGameInstance::SessionSetup()
{
	//DeleteSave(ESaveGameType::Session); @CLEAN
	// SessionManager = NewObject<USessionManager>(this);
	// SessionManager->Setup();
}

void USkyGameInstance::SessionStart()
{
	// SessionSetup();
	//
	// APlayerController* controller = GEngine->GetFirstLocalPlayerController(GetWorld());
	// ASkyHUD* hud = controller->GetHUD<ASkyHUD>();
	// hud->ShowGameOverlay();
	//
	// check(SessionManager);
	// SessionManager->Start();
	//
	// SessionStartDelegate.Broadcast();
}

void USkyGameInstance::SessionClear()
{
	//SessionManager->MarkAsGarbage();
	//SessionManager = nullptr;
}

// USessionRules* USkyGameInstance::GetRules()
// {
// 	return SessionRules;
// }

//@TODO
//
// void USkyGameInstance::SetAudio_Master(float value)
// {
// 	AppSettings.Audio_Master = value;
// }
//
// void USkyGameInstance::SetAudio_Music(float value)
// {
// 	AppSettings.Audio_Music = value;
// }
//
// void USkyGameInstance::SetAudio_UI(float value)
// {
// 	AppSettings.Audio_UI = value;
// }
//
// void USkyGameInstance::SetAudio_Environment(float value)
// {
// 	AppSettings.Audio_Environment = value;
// }
//
// void USkyGameInstance::SetFOV(float value)
// {
// 	AppSettings.FOV = value;
// }
