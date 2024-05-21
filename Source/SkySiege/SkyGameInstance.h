// Property of Looch Labs LLC

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "SkyGameInstance.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSessionStartDelegate);



UCLASS()
class SKYSIEGE_API USkyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void Setup();

	UFUNCTION(BlueprintCallable)
	void SessionSetup();
	
	UFUNCTION(BlueprintCallable)
	void SessionStart();
	
	UFUNCTION(BlueprintCallable)
	void SessionClear();

	UFUNCTION(BlueprintCallable)
	static bool IsDebugMode() { return DebugMode; }
	
	static const bool DebugMode;

	UPROPERTY(BlueprintReadWrite)
	FString VersionLabel;
	
	UPROPERTY(BlueprintReadWrite)
	FString ChangelogLabel;
	
	// Configs
	UPROPERTY(BlueprintReadWrite)
	FName MainLevelName;

	//@TODO
	//UPROPERTY(BlueprintReadOnly)
	//FName SocialURL_Discord;
	//
	//UPROPERTY(BlueprintReadOnly)
	//FName SocialURL_Twitter;
	//
	//UPROPERTY(BlueprintReadOnly)
	//FName SocialURL_Twitch;
	//
	//UPROPERTY(BlueprintReadOnly)
	//FName SocialURL_Steam;

	//@TODO
	//UPROPERTY(BlueprintReadOnly)
	//FAppSettings AppSettings;

	//UPROPERTY()
	//USessionRules* SessionRules;
	
	//UPROPERTY(BlueprintReadWrite)
	//USessionManager* SessionManager = nullptr;
	//
	//UPROPERTY(BlueprintAssignable)
	//FSessionStartDelegate SessionStartDelegate;
};


