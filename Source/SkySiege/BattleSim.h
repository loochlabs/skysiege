// Property of Looch Labs LLC.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnitStats.h"
#include "UObject/NoExportTypes.h"
#include "BattleSim.generated.h"


class AGridUnitActor;
struct FBattleSimulation;


UENUM(BlueprintType)
enum class EBattleID : uint8
{
	None = 0, 
	A,
	B,
};

UENUM(BlueprintType)
enum class EBattleUnitStepResult : uint8
{
	None = 0,
	WaitingForCooldown,
	ActionSuccess,
	ActionFail,
	NotEnoughFood,
};


UENUM(BlueprintType)
enum class EBattleEventID : uint8
{
	None = 0,
	ActionMissed,
	AddMaxHP,
	RemoveMaxHP,
	AddHP,
	RemoveHP,
	FatigueDamage,
	AddFood,
	RemoveFood,
	NotEnoughFood,
	
	MatchWinner,
};


USTRUCT(BlueprintType)
struct FBattleUnit
{
	GENERATED_BODY();
	
	void Start(FBattleSimulation& Sim);
	EBattleUnitStepResult Step(FBattleSimulation& Sim);

	void Bonus_AddPower(FBattleSimulation& Sim);
	
	void StartAction_AddMaxHP_1000(FBattleSimulation& Sim);
	void StartAction_AddFood_25(FBattleSimulation& Sim);
	
	void Action_DamageEnemy(FBattleSimulation& Sim);
	void Action_AddFood(FBattleSimulation& Sim);
	void Action_AddHP(FBattleSimulation& Sim);

	UPROPERTY(BlueprintReadOnly)
	EBattleID Owner = EBattleID::None;

	UPROPERTY(BlueprintReadOnly)
	int32 UnitID = -1;

	UPROPERTY(BlueprintReadOnly)
	FName UnitKey;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTagContainer Tags;

	UPROPERTY(BlueprintReadOnly)
	FUnitStats Stats;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentCooldown = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 StartTime = 0;
};

USTRUCT(BlueprintType)
struct FBattleProfile
{
	GENERATED_BODY()
	
	void ResetStats();
	void Start(FBattleSimulation& Sim);
	void Step(FBattleSimulation& Sim);

	void AddMaxHP(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount);
	void RemoveMaxHP(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount);
	void AddHP(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount);
	void RemoveHP(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount);
	void DealFatigueDamage(FBattleSimulation& Sim);
	void AddFood(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount);
	void RemoveFood(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount);

	UPROPERTY(BlueprintReadOnly)
	EBattleID ID = EBattleID::None;
	
	UPROPERTY(BlueprintReadOnly)
	int32 HP = 0;
	
	UPROPERTY(BlueprintReadOnly)
	int32 MaxHP = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Food = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 FatigueTime = 1000;

	UPROPERTY(BlueprintReadOnly)
	int32 FatigueTimeRate = 1000;

	UPROPERTY(BlueprintReadOnly)
	int32 FatigueDamage = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 FatigueDamageIncrement = 1;

	UPROPERTY(BlueprintReadOnly)
	TArray<FBattleUnit> Units;
	
	UPROPERTY(BlueprintReadOnly)
	FString PlayerName = TEXT("Default Name");
};

USTRUCT(BlueprintType)
struct FBattleEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EBattleEventID EventID = EBattleEventID::None;

	UPROPERTY(BlueprintReadOnly)
	EBattleID OwnerID = EBattleID::None;

	UPROPERTY(BlueprintReadOnly)
	int32 SourceUnitID = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Amount = 0;
};

USTRUCT(BlueprintType)
struct FBattleFrame
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 TimeMS = 0;

	UPROPERTY(BlueprintReadOnly)
	FBattleProfile UserA;

	UPROPERTY(BlueprintReadOnly)
	FBattleProfile UserB;

	UPROPERTY(BlueprintReadOnly)
	TArray<FBattleEvent> Events;
};

USTRUCT(BlueprintType)
struct FBattleSimulation
{
	GENERATED_BODY()
	
	FBattleProfile& GetProfile(EBattleID ID);
	FBattleProfile& GetEnemyProfileOf(EBattleID ID);

	void Start();
	void Record();
	void AddEvent(EBattleEventID EventID, EBattleID ID, int32 SourceUnitID, int32 Amount);
	
	FBattleProfile UserA;
	FBattleProfile UserB;
	int32 TimeMS = 0;
	EBattleID Winner = EBattleID::None;
	
	// Map our unique battle unit ID to an actor pointer
	UPROPERTY()
	TMap<int32, AGridUnitActor*> BattleUnitActors;
	
	TArray<FBattleEvent> FrameEvents;
	TArray<FBattleFrame> SavedFrames;

	void SetTag(FGameplayTag& Tag, FName&& TagName)
	{
		Tag = FGameplayTag::RequestGameplayTag(TagName);
	}

	void FillTags()
	{
		SetTag(UnitTag_StartAction_AddMaxHP_1000, "Unit.StartAction.AddMaxHP_1000");
		SetTag(UnitTag_StartAction_AddFood_25, "Unit.StartAction.AddFood_25");
		
		SetTag(UnitTag_Bonus_AddPower, "Unit.Bonus.AddPower");

		SetTag(UnitTag_Action_DamageEnemy, "Unit.Action.DamageEnemy");
		SetTag(UnitTag_Action_AddFood, "Unit.Action.AddFood");
		SetTag(UnitTag_Action_AddHP, "Unit.Action.AddHP");
	}
	
	FGameplayTag UnitTag_StartAction_AddMaxHP_1000;
	FGameplayTag UnitTag_StartAction_AddFood_25;
	
	FGameplayTag UnitTag_Bonus_AddPower;
	
	FGameplayTag UnitTag_Action_DamageEnemy;
	FGameplayTag UnitTag_Action_AddFood;
	FGameplayTag UnitTag_Action_AddHP;
};
