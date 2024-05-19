// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "SessionConfig.h"
#include "SessionPhase.h"
#include "GameFramework/GameModeBase.h"
#include "SkyGameModeBase.generated.h"

class UUserProfile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkyUpdatedPhaseDelegate, ESessionPhase, Phase);


UCLASS()
class SKYSIEGE_API ASkyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure,  meta=(WorldContext="WorldContextObject"))
	static ASkyGameMode* Get(UObject* WorldContextObject);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	const FUnitTemplate& GetUnitTemplate(const FName& UnitKey);

	void StartGame();

	UFUNCTION(BlueprintCallable)
	void StartPhase(ESessionPhase Phase);	
	
	void StartBattleSim();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FSessionConfig Config;
	
	UPROPERTY(BlueprintReadOnly)
	UUserProfile* UserProfile = nullptr;

	UPROPERTY(BlueprintReadOnly)
	UUserProfile* EnemyProfile = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	ESessionPhase Phase = ESessionPhase::None;	

	UPROPERTY(BlueprintAssignable)
	FSkyUpdatedPhaseDelegate OnUpdatedPhase;
};
