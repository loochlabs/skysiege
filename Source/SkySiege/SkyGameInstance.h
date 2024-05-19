// Property of Looch Labs LLC

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

//#include "RulesConfig.h"
//#include "SessionManager.h"
//#include "Http.h" @CLEAN
///#include "SettingsSaveGame.h"
///#include "CareerSaveGame.h"
///#include "SessionSaveGame.h"

#include "SkyGameInstance.generated.h"

//class USessionManager;

//class URulesConfig;
//struct FUnitTemplate;
//class AGridUnitActor;
//class USessionRules;
//class USoundCue; @CLEAN
//class AMusicManager;


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
	
	//USessionRules* GetRules();

	//@TODO
	//UFUNCTION(BlueprintCallable) void SetAudio_Master(float value);
	//UFUNCTION(BlueprintCallable) void SetAudio_Music(float value);
	//UFUNCTION(BlueprintCallable) void SetAudio_UI(float value);
	//UFUNCTION(BlueprintCallable) void SetAudio_Environment(float value);
	//UFUNCTION(BlueprintCallable) void SetFOV(float value);

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


