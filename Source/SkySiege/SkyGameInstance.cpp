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
