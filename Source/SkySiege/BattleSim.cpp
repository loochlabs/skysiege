// Property of Looch Labs LLC.


#include "BattleSim.h"

//--------------------------
//	Battle Units
//--------------------------

void FBattleUnit::Start(FBattleSimulation& Sim)
{
	StartTime = FMath::Rand() % 500;

	// do start actions, bonuses, statuses
	TArray<FGameplayTag> tags;
	Tags.GetGameplayTagArray(tags);
	for(const FGameplayTag& tag : tags)
	{
		if(Sim.TagStartActions.Contains(tag))
			Sim.TagStartActions[tag](*this, Sim);
	}
}

EBattleUnitStepResult FBattleUnit::Step(FBattleSimulation& Sim)
{
	if(Sim.TimeMS < StartTime)
	{
		return EBattleUnitStepResult::WaitingForCooldown;
	}
	
	if(CurrentCooldown < Stats.Cooldown)
	{
		CurrentCooldown++;
		return EBattleUnitStepResult::WaitingForCooldown;
	}

	CurrentCooldown = 0;

	// food
	auto& profile = Sim.GetProfile(Owner);
	if(Stats.Cost > 0 && profile.Food < Stats.Cost)
	{
		Sim.AddEvent(EBattleEventID::NotEnoughFood, Owner, UnitID, Stats.Cost);
		return EBattleUnitStepResult::NotEnoughFood;
	}

	profile.RemoveFood(Sim, UnitID, Stats.Cost);
	
	// actions
	if(FMath::FRand() < Stats.Competence)
	{
		// do actions
		TArray<FGameplayTag> tags;
		Tags.GetGameplayTagArray(tags);
		for(const FGameplayTag& tag : tags)
		{
			if(Sim.TagActions.Contains(tag))
				Sim.TagActions[tag](*this, Sim);
		}

		return EBattleUnitStepResult::ActionSuccess;
	}
	
	Sim.AddEvent(EBattleEventID::ActionMissed, Owner, UnitID, Stats.Cost);
	return EBattleUnitStepResult::ActionFail;
}

// void FBattleUnit::Bonus_AddPower(FBattleSimulation& Sim)
// {
// 	Stats.Power += 50;
// }

//@CLEAN
// void FBattleUnit::StartAction_AddMaxHP_1000(FBattleSimulation& Sim)
// {
// 	Sim.GetProfile(Owner).AddMaxHP(Sim, UnitID, Stats.Power * 16);
// }
//
// void FBattleUnit::StartAction_AddFood_25(FBattleSimulation& Sim)
// {
// 	Sim.GetProfile(Owner).AddFood(Sim, UnitID, Stats.Power); 
// }

// void FBattleUnit::Action_DamageEnemy(FBattleSimulation& Sim)
// {
// 	Sim.GetEnemyProfileOf(Owner).RemoveHP(Sim, UnitID, Stats.Power);
// }
//
// void FBattleUnit::Action_AddFood(FBattleSimulation& Sim)
// {
// 	Sim.GetProfile(Owner).AddFood(Sim, UnitID, Stats.Power);
// }
//
// void FBattleUnit::Action_AddHP(FBattleSimulation& Sim)
// {
// 	Sim.GetProfile(Owner).AddHP(Sim, UnitID, Stats.Power);
// }

//--------------------------
//	Battle PROFILES
//--------------------------

void FBattleProfile::ResetStats()
{
	HP = MaxHP;
}

void FBattleProfile::Start(FBattleSimulation& Sim)
{
	for(auto& unit : Units)
	{
		unit.Start(Sim);
	}
}

void FBattleProfile::Step(FBattleSimulation& Sim)
{
	if(Sim.TimeMS % FoodGenerationRate == 0)
	{
		AddFood(Sim, 0, FoodGeneration);
	}
	
	for(auto& unit : Units)
	{
		unit.Step(Sim);
	}

	if(Sim.TimeMS >= FatigueTime)
	{
		DealFatigueDamage(Sim);
		FatigueTime = Sim.TimeMS + FatigueTimeRate;  
	}
}

void FBattleProfile::AddMaxHP(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount)
{
	if(Amount <= 0) return;
	
	MaxHP = MaxHP + Amount;
	Sim.AddEvent(EBattleEventID::AddMaxHP, ID, SourceUnitID, Amount);
}

void FBattleProfile::RemoveMaxHP(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount)
{
	if(Amount <= 0) return;
	
	MaxHP = FMath::Max(MaxHP - Amount, 1);
	Sim.AddEvent(EBattleEventID::RemoveMaxHP, ID, SourceUnitID, Amount);
}

void FBattleProfile::AddHP(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount)
{
	if(Amount <= 0) return;
	
	HP = FMath::Clamp(HP + Amount, HP, MaxHP);
	Sim.AddEvent(EBattleEventID::AddHP, ID, SourceUnitID, Amount);
}

void FBattleProfile::RemoveHP(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount)
{
	if(Amount <= 0) return;
	
	HP = FMath::Clamp(HP - Amount, 0, MaxHP);
	Sim.AddEvent(EBattleEventID::RemoveHP, ID, SourceUnitID, Amount);
}

void FBattleProfile::DealFatigueDamage(FBattleSimulation& Sim)
{
	FatigueDamage += FatigueDamageIncrement;
	HP = FMath::Clamp(HP - FatigueDamage, 0, MaxHP);
	Sim.AddEvent(EBattleEventID::FatigueDamage, ID, 0, FatigueDamage);
}

void FBattleProfile::AddFood(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount)
{
	if(Amount <= 0) return;
	
	Food += Amount;
	Sim.AddEvent(EBattleEventID::AddFood, ID, SourceUnitID, Amount);
}

void FBattleProfile::RemoveFood(FBattleSimulation& Sim, int32 SourceUnitID, int32 Amount)
{
	if(Amount <= 0) return;
	
	Food = FMath::Max(Food - Amount, 0);
	Sim.AddEvent(EBattleEventID::RemoveFood, ID, SourceUnitID, Amount);
}


//--------------------------
//	Battle Sim
//--------------------------

FBattleProfile& FBattleSimulation::GetProfile(EBattleID ID)
{
	check(ID != EBattleID::None);
	switch(ID)
	{
	case EBattleID::B: return UserB;
	case EBattleID::A:
	default:
		return UserA;
	}
}

FBattleProfile& FBattleSimulation::GetEnemyProfileOf(EBattleID ID)
{
	check(ID != EBattleID::None);
	switch(ID)
	{
	case EBattleID::B: return UserA;
	case EBattleID::A:
	default:
		return UserB;
	}
}

void FBattleSimulation::Start()
{
	// Phase 0: On Start on all profile Units
	//@DESIGN: need to make sure player profiles do not effect each other yet?
	TimeMS = 0;
	FillTags();

	// Phase 1: Start
	UserA.Start(*this);
	UserB.Start(*this);

	// Phase 1b: Reset Max HP
	UserA.ResetStats();
	UserB.ResetStats();

	Record();
	
	// Phase 2: Step Sim
	while(UserA.HP > 0 && UserB.HP > 0)
	{
		TimeMS++;
		UserA.Step(*this);
		UserB.Step(*this);
		Record();
	}

	// Phase 3: Results
	TimeMS++;
	if(UserA.HP > 0)
		Winner = EBattleID::A;
	else if(UserB.HP > 0)
		Winner = EBattleID::B;

	AddEvent(EBattleEventID::MatchWinner, Winner, 0, 0);
	Record();
}

void FBattleSimulation::Record()
{
	FBattleFrame frame;
	frame.TimeMS = TimeMS;
	frame.UserA = UserA;
	frame.UserB = UserB;
	frame.Events = FrameEvents;
	FrameEvents.Reset();
	SavedFrames.Add(frame);
}

void FBattleSimulation::AddEvent(EBattleEventID EventID, EBattleID ID, int32 SourceUnitID, int32 Amount)
{
	FBattleEvent event;
	event.EventID = EventID;
	event.OwnerID = ID;
	event.SourceUnitID = SourceUnitID;
	event.Amount = Amount;
	FrameEvents.Add(event);
}

void FBattleSimulation::FillTags()
{
	// ~~~~~ Start Actions ~~~~~
	auto add_start_action = [&](FName&& TagName, UnitBehavior Func)
	{
		TagStartActions.Add(FGameplayTag::RequestGameplayTag(TagName), Func);
	};

	add_start_action("Unit.StartAction.AddFood", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Sim.GetProfile(Unit.Owner).AddFood(Sim, Unit.UnitID, Unit.Stats.Power); 
	} );

	// ~~~~~ Actions ~~~~~
	auto add_action = [&](FName&& TagName, UnitBehavior Func)
	{
		TagActions.Add(FGameplayTag::RequestGameplayTag(TagName), Func);
	};
	
	// Jobs
	add_action("Unit.Job.Farmer", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Sim.GetProfile(Unit.Owner).AddFood(Sim, Unit.UnitID, Unit.Stats.Power);
	} );

	add_action("Unit.Job.Soldier", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Sim.GetEnemyProfileOf(Unit.Owner).RemoveHP(Sim, Unit.UnitID, Unit.Stats.Power);
	} );

	add_action("Unit.Job.Engineer", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Sim.GetProfile(Unit.Owner).AddHP(Sim, Unit.UnitID, Unit.Stats.Power);
	} );
}