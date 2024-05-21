// Property of Looch Labs LLC.


#include "SkyGameModeBase.h"

#include "BattleSim.h"
#include "GridCellActor.h"
#include "GridUnitActor.h"
#include "SkyGrid.h"
#include "SkyHUD.h"
#include "UnitTemplate.h"
#include "UserProfile.h"
#include "Kismet/GameplayStatics.h"
#include "SkyLog.h"

ASkyGameMode* ASkyGameMode::Get(const UObject* WorldContext)
{
	return Cast<ASkyGameMode>(UGameplayStatics::GetGameMode(WorldContext));
}


void ASkyGameMode::BeginPlay()
{
	Super::BeginPlay();

	Config.Setup();
	
	APlayerController* controller = GEngine->GetFirstLocalPlayerController(GetWorld());
	ASkyHUD* hud = controller->GetHUD<ASkyHUD>();
	hud->ShowMainMenu();
}

const FUnitTemplate& ASkyGameMode::GetUnitTemplate(const FName& UnitKey)
{
	if(FUnitTemplate* unitTemplate = Config.UnitTemplates.Find(UnitKey))
	{
		return *unitTemplate;
	}
	check(false);
	static FUnitTemplate temp;
	return temp;
}

void ASkyGameMode::StartGame()
{
	check(!IsValid(UserProfile));
	UserProfile = NewObject<UUserProfile>(this);
	FUserProfileConfig profileCfg;
	profileCfg.GridPadding = 15;
	
	// starting units
	profileCfg.AddStartingUnit("unit_land_basic", 2, 2);
	profileCfg.AddStartingUnit("unit_human_peon", 2, 2);
	profileCfg.AddStartingUnit("unit_human_brute", 2, 3);
	
	UserProfile->Setup(profileCfg);

	//@TODO this is for camera positioning, get this out of the Grid
	UserProfile->Grid->SetViewToMacro();

	StartPhase(ESessionPhase::Shop);
}

void ASkyGameMode::StartPhase(ESessionPhase InPhase)
{
	Phase = InPhase;
	UserProfile->StartPhase(InPhase);
	OnUpdatedPhase.Broadcast(InPhase);

	switch(InPhase)
	{
	case ESessionPhase::Shop:
		// teardown existing battle profiles
		if(IsValid(EnemyProfile))
		{
			EnemyProfile->Teardown();
			EnemyProfile = nullptr;
		}
		
	{
		// check new day tags
		FGameplayTag tagExercising = FGameplayTag::RequestGameplayTag("Unit.Status.Exercising");
		FGameplayTag tagStrong = FGameplayTag::RequestGameplayTag("Unit.Bonus.Strong");
		for(auto& cellPair : UserProfile->Grid->Cells)
		{
			for(auto& unit : cellPair.Value->UnitActors)
			{
				if(unit->UnitTags.HasTag(tagExercising))
					unit->UnitTags.AddTag(tagStrong);
			}
		}
	}
		
		break;
	
	case ESessionPhase::Battle:
		StartBattleSim();
		break;
	
	default:
		break;
	}
}

void ASkyGameMode::StartBattleSim()
{
	FBattleSimulation battle = FBattleSimulation();

	// fill battle profile
	auto create_profile = [&](EBattleID ID, UUserProfile* Profile)
	{
		// get all units in local user profile
		TMap<int32, AGridUnitActor*> unitActors;
		for(auto& cellPair : Profile->Grid->Cells)
		{
			AGridCellActor* cell = cellPair.Value;
			for(auto& unitActor : cell->UnitActors)
			{
				if(unitActors.Contains(unitActor->UnitID))
					continue;

				unitActors.Add(unitActor->UnitID, unitActor);
			}
		}
		
		FBattleProfile profile;
		profile.ID = ID;
		profile.MaxHP = 1000;
		static int32 unitID = 0;
		for(auto& unitPair : unitActors)
		{
			FBattleUnit unit;
			unit.UnitID = ++unitID;
			battle.BattleUnitActors.Add(unit.UnitID, unitPair.Value);
			
			unit.Owner = profile.ID;
			unit.UnitKey = unitPair.Value->UnitKey;
			unit.Tags = unitPair.Value->UnitTags;

			auto& unitTemplate = GetUnitTemplate(unit.UnitKey);
			unit.Stats = unitTemplate.UnitStats;
			
			profile.Units.Add(unit);
		}
		return profile;
	};
	battle.UserA = create_profile(EBattleID::A, UserProfile);
	battle.UserA.PlayerName = TEXT("You");

	// build mock profiles
	EnemyProfile = NewObject<UUserProfile>(this);
	FUserProfileConfig profileCfg;
	profileCfg.GridLocation = FVector(-500, 1400, 10);
	profileCfg.GridRotation = FRotator(0, 180.f, 0);
	profileCfg.AddStartingUnit("unit_land_basic", 0, 0);
	profileCfg.AddStartingUnit("unit_land_basic", 2, 0);
	profileCfg.AddStartingUnit("unit_land_basic", 4, 0);
	profileCfg.AddStartingUnit("unit_land_basic", 0, 2);
	profileCfg.AddStartingUnit("unit_land_basic", 2, 2);
	profileCfg.AddStartingUnit("unit_land_basic", 4, 2);
	profileCfg.AddStartingUnit("unit_human_peon", 2, 2);
	profileCfg.AddStartingUnit("unit_human_brute", 2, 3);
	EnemyProfile->Setup(profileCfg);
	battle.UserB = create_profile(EBattleID::B, EnemyProfile);
	
	battle.Start();

	switch(battle.Winner)
	{
	case EBattleID::A:
		UserProfile->CurrentWins++;
		break;

	case EBattleID::B:
		UserProfile->CurrentLoses++;
		break;

	default:
		break;
	}

	// display result
	APlayerController* controller = GEngine->GetFirstLocalPlayerController(GetWorld());
	ASkyHUD* hud = controller->GetHUD<ASkyHUD>();
	hud->ShowBattle(battle);
	
	//@CLEAN 
	//UE_LOG(SkyLog, Log, TEXT("Battle sim complete. sizeof=%d"), sizeof(battle));
}
