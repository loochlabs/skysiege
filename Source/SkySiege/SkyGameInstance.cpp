// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyGameInstance.h"


#if UE_EDITOR + UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT != 0 
	const bool USkyGameInstance::DebugMode = true;
 #else
 	const bool USkyGameInstance::DebugMode = false;
#endif


void USkyGameInstance::Setup()
{
	// Display text pulled from txt's
	FString versionPath = FPaths::ProjectContentDir();
	versionPath += FString(TEXT("App/version.txt"));
	FFileHelper::LoadFileToString(VersionLabel, *versionPath);

	// Display text pulled from txt's
	FString changelogPath = FPaths::ProjectContentDir();
	changelogPath += FString(TEXT("App/changelog.txt"));
	FFileHelper::LoadFileToString(ChangelogLabel, *changelogPath);
}
