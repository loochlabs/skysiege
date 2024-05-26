// Property of Looch Labs LLC

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "SkyGameInstance.generated.h"



UCLASS()
class SKYSIEGE_API USkyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void Setup();

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
};


