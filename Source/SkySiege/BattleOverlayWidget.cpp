// Property of Looch Labs LLC.


#include "BattleOverlayWidget.h"

#include "GridUnitActor.h"


void UBattleOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!bShowSim)
		return;

	for(int32 n = 0; n < FramesProcessedPerTick; ++n)
	{
		SimFrame++;

		if(SimFrame >= Sim.SavedFrames.Num())
		{
			bShowSim = false;
			return;
		}

		DisplayFrame(SimFrame);
	}
}

void UBattleOverlayWidget::ShowSim(const FBattleSimulation& InSim)
{
	bShowSim = true;
	Sim = InSim;
	SimFrame = -1;
	FramesProcessedPerTick = DefaultFramesProcessedPerTick;
}

void UBattleOverlayWidget::DisplayFrame(int32 FrameTime)
{
	if(FrameTime < 0 || FrameTime >= Sim.SavedFrames.Num())
		return;
	
	const FBattleFrame& frame = Sim.SavedFrames[SimFrame];
	OnDisplayFrame(frame);

	// event processing
	for(auto& event : frame.Events)
	{
		int32 unitID = event.SourceUnitID;

		if(!Sim.BattleUnitActors.Contains(unitID))
			continue;
		
		AGridUnitActor* unitActor = Sim.BattleUnitActors[unitID];
		unitActor->HandleBattleEvent(event);
	}
}

