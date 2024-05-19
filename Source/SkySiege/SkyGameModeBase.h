// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "SessionConfig.h"
#include "SessionPhase.h"
#include "GameFramework/GameModeBase.h"
#include "SkyGameModeBase.generated.h"

class UUserProfile;


UCLASS()
class SKYSIEGE_API ASkyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure,  meta=(WorldContext="WorldContextObject"))
	static ASkyGameMode* Get(UObject* WorldContext);

	virtual void BeginPlay() override;

	const FUnitTemplate& GetUnitTemplate(const FName& UnitKey);

	void StartGame();

	UFUNCTION(BlueprintCallable)
	void StartPhase(ESessionPhase Phase);	
	
	void StartBattleSim();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSessionConfig Config;

	//@CLEAN
	//UFUNCTION(BlueprintPure)
	//static USessionProfile* GetUserProfile(UObject* WorldContext);
	
	UPROPERTY(BlueprintReadOnly)
	UUserProfile* UserProfile = nullptr;

	UPROPERTY(BlueprintReadOnly)
	UUserProfile* EnemyProfile = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	ESessionPhase Phase = ESessionPhase::None;	

	//@CLEAN
	// delegates
	//UPROPERTY(BlueprintAssignable)
	//FSessionUpdatedPhaseDelegate OnUpdatedPhase;
};
