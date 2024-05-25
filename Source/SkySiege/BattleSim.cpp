// Property of Looch Labs LLC.


#include "BattleSim.h"

//--------------------------
//	Battle Units
//--------------------------

void FBattleUnit::Start(FBattleSimulation& Sim)
{
	StartTime = FMath::Rand() % 500;

	// do bonuses, statuses
	TArray<FGameplayTag> tagStatuses;
	Tags.GetGameplayTagArray(tagStatuses);
	for(const FGameplayTag& tag : tagStatuses)
	{
		if(Sim.TagStatus.Contains(tag))
			Sim.TagStatus[tag].Behavior(*this, Sim);
	}
	
	// do start actions, bonuses, statuses
	TArray<FGameplayTag> tags;
	Tags.GetGameplayTagArray(tags);
	for(const FGameplayTag& tag : tags)
	{
		if(Sim.TagStartActions.Contains(tag))
			Sim.TagStartActions[tag].Behavior(*this, Sim);
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
				Sim.TagActions[tag].Behavior(*this, Sim);
		}

		return EBattleUnitStepResult::ActionSuccess;
	}
	
	Sim.AddEvent(EBattleEventID::ActionMissed, Owner, UnitID, Stats.Cost);
	return EBattleUnitStepResult::ActionFail;
}

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

TMap<FGameplayTag, FBattleSimulation::FUnitBehaviorData> FBattleSimulation::TagStatus;
TMap<FGameplayTag, FBattleSimulation::FUnitBehaviorData> FBattleSimulation::TagStartActions;
TMap<FGameplayTag, FBattleSimulation::FUnitBehaviorData> FBattleSimulation::TagActions;

FString FBattleSimulation::GetTagDescriptionRaw(const FGameplayTag& Tag)
{
	if(TagStatus.Contains(Tag))
	{
		return FString::Printf(TEXT("%s %s"), *TagStatus[Tag].Label, *TagStatus[Tag].DescriptionRaw);
	}
	if(TagStartActions.Contains(Tag))
	{
		return FString::Printf(TEXT("%s %s"), *TagStartActions[Tag].Label, *TagStartActions[Tag].DescriptionRaw);
	}
	if(TagActions.Contains(Tag))
	{
		return FString::Printf(TEXT("%s %s"), *TagActions[Tag].Label, *TagActions[Tag].DescriptionRaw);
	}
	return "";
}

FText FBattleSimulation::GetTagDescriptionFormatted(const FGameplayTag& Tag, const FUnitStats& Stats)
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("StatPower"), Stats.Power);
	Args.Add(TEXT("StatCost"), Stats.Cost);
	Args.Add(TEXT("StatCooldown"), static_cast<float>(Stats.Cooldown) / 1000.f);
	Args.Add(TEXT("StatCompetence"), Stats.Competence);

	Args.Add(TEXT("Power"), FText::FromString("<img id=\"Power\"/>"));
	Args.Add(TEXT("Food"), FText::FromString("<img id=\"Food\"/>"));
	Args.Add(TEXT("Cooldown"), FText::FromString("<img id=\"Cooldown\"/>"));
	Args.Add(TEXT("Competence"), FText::FromString("<img id=\"Competence\"/>"));
	Args.Add(TEXT("Damage"), FText::FromString("<img id=\"Damage\"/>"));
	Args.Add(TEXT("Repair"), FText::FromString("<img id=\"Repair\"/>"));
	Args.Add(TEXT("NoFood"), FText::FromString("<img id=\"NoFood\"/>"));
	Args.Add(TEXT("ActionMissed"), FText::FromString("<img id=\"ActionMissed\"/>"));
	Args.Add(TEXT("Health"), FText::FromString("<img id=\"Health\"/>"));
	Args.Add(TEXT("Land"), FText::FromString("<img id=\"Land\"/>"));
	Args.Add(TEXT("Building"), FText::FromString("<img id=\"Building\"/>"));
	Args.Add(TEXT("Worker"), FText::FromString("<img id=\"Worker\"/>"));
	Args.Add(TEXT("Storage"), FText::FromString("<img id=\"Storage\"/>"));

	FString descriptionRaw = GetTagDescriptionRaw(Tag);
	FText descRawText = FText::FromString(descriptionRaw);
	return FText::Format(descRawText, Args);
}

void FBattleSimulation::FillTags()
{
	TagStatus.Empty();
	TagStartActions.Empty();
	TagActions.Empty();

	// ~~~~~ Statuses ~~~~~
	auto add_status = [&](FName&& TagName, UnitBehavior Func, FString&& Label, FString&& Description)
	{
		FUnitBehaviorData data;
		data.Behavior = Func;
		data.Label = Label;
		data.DescriptionRaw = Description;
		FGameplayTag tag = FGameplayTag::RequestGameplayTag(TagName);
		TagStatus.Add(tag, data);
	};
	
	// ~~~~~ Start Actions ~~~~~
	auto add_start_action = [&](FName&& TagName, UnitBehavior Func, FString&& Label, FString&& Description)
	{
		FUnitBehaviorData data;
		data.Behavior = Func;
		data.Label = Label;
		data.DescriptionRaw = Description;
		FGameplayTag tag = FGameplayTag::RequestGameplayTag(TagName);
		TagStartActions.Add(tag, data);
	};
	
	// ~~~~~ Actions ~~~~~
	auto add_action = [&](FName&& TagName, UnitBehavior Func, FString&& Label, FString&& Description)
	{
		FUnitBehaviorData data;
		data.Behavior = Func;
		data.Label = Label;
		data.DescriptionRaw = Description;
		FGameplayTag tag = FGameplayTag::RequestGameplayTag(TagName);
		TagActions.Add(tag, data);
	};
	
	add_start_action("Unit.StartAction.AddFood", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Sim.GetProfile(Unit.Owner).AddFood(Sim, Unit.UnitID, Unit.Stats.Power);
	},
	"",
	"Add {StatPower} {Food}");


	// -------- Statuses -----------

	add_status("Unit.Status.Exercising", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Power *= 1.1f;
	},
	"<HighlightGreen>[Exercising]</>",
	"+10% action output.");
	
	add_status("Unit.Status.Strong", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Power *= 1.5f;
	},
	"<HighlightGreen>[Strong]</>",
	"+50% action output.");
	
	add_status("Unit.Status.Bulky", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Power *= 3.f;
		Unit.Stats.Cost *= 2.0f;
	},
	"<HighlightGreen>[Bulky]</>",
	"+200% action output. +200% consumption");
	
	add_status("Unit.Status.MeatHead", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Power *= 5.f;
		Unit.Stats.Cost *= 5.0f;
	},
	"<HighlightGreen>[Meat Head]</>",
	"+400% action output. +400% consumption");
	
	add_status("Unit.Status.Burning", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Power *= 0.9f;
	},
	"<HighlightGreen>[Burning]</>",
	"-10% action output.");

	add_status("Unit.Status.Cozy", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Power *= 1.1f;
	},
	"<HighlightGreen>[Cozy]</>",
	"+10% action output");
	
	add_status("Unit.Status.Eating", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Cost *= 0.9f;
	},
	"<HighlightGreen>[Eating]</>",
	"-10% consumption.");

	add_status("Unit.Status.Fed", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Cost *= 0.75f;
	},
	"<HighlightGreen>[Fed]</>",
	"-25% consumption.");

	add_status("Unit.Status.Overate", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Cost *= 0.25f;
	},
	"<HighlightGreen>[Overate]</>",
	"-75% consumption.");

	add_status("Unit.Status.Glutton", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Cost = 0;
		Unit.Stats.Cooldown *= 3.f;
	},
	"<HighlightGreen>[Glutton]</>",
	"-100% consumption. +200% {Cooldown}");

	// Elements
	add_status("Unit.Element.Fire", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Competence *= 0.9f;
	},
	"<HighlightGreen>[Hot]</>",
	"-10% {Competence}");

	add_status("Unit.Element.Ice", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Cooldown *= 1.1f;
	},
	"<HighlightGreen>[Cold]</>",
	"+10% {Cooldown}");
	
	add_status("Unit.Element.Nature", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Power *= 0.9f;
	},
	"<HighlightGreen>[Relaxed]</>",
	"-10% {Power}");

	add_status("Unit.Element.Death", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Unit.Stats.Cost *= 1.1f;
	},
	"<HighlightGreen>[Sick]</>",
	"+10% consumption");
	

	// Jobs
	add_action("Unit.Job.Farmer", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Sim.GetProfile(Unit.Owner).AddFood(Sim, Unit.UnitID, Unit.Stats.Power);
	},
	"<HighlightOrange>[Farmer]</>",
	"Gain {StatPower} {Food} every {StatCooldown} sec. Eat {StatCost} {Food}.");
	
	add_action("Unit.Job.Soldier", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Sim.GetEnemyProfileOf(Unit.Owner).RemoveHP(Sim, Unit.UnitID, Unit.Stats.Power);
	},
	"<HighlightOrange>[Soldier]</>",
	"Deal {StatPower} {Damage} every {StatCooldown} sec. Eat {StatCost} {Food}.");

	add_action("Unit.Job.Engineer", [](FBattleUnit& Unit, FBattleSimulation& Sim)
	{
		Sim.GetProfile(Unit.Owner).AddHP(Sim, Unit.UnitID, Unit.Stats.Power);
	},
	"<HighlightOrange>[Engineer]</>",
	"Repair {StatPower} {Health} every {StatCooldown} sec. Eat {StatCost} {Food}.");
}
