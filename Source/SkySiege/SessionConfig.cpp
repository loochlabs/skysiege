// Property of Looch Labs LLC.


#include "SessionConfig.h"

#include "UnitDataTableRow.h"


void FSessionConfig::Setup()
{
	check(UnitTemplateData);

	UnitTemplates.Reset();
	for (auto& it : UnitTemplateData->GetRowMap())
	{
		FUnitDataTableRow* row = (FUnitDataTableRow*)(it.Value);
		UnitTemplates.Add(it.Key, row->UnitTemplate);
	}
}
