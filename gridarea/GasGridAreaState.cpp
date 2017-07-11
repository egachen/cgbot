#include "GasGridAreaState.h"
#include "GasGridArea.h"

namespace CgBot
{
	DEFINE_OBJECT(GasBuilding, GridArea)
	void GasBuilding::Execute(GridArea* area) {
		// in this sate, check if builder does not build, to avoid gridarea stuck in this state
		if (area->resourceQuota_.isAllocated()) {
			if (area->getBuilder()->isIdle()) {
				if (area->toBuildQueue_.front().isRefinery()){
					// since there is no oncreate event for refinery
					area->getBuilder()->gather(area->getBuilder()->getClosestUnit(BWAPI::Filter::IsMineralField));
					area->resourceQuota_.resetQuota();
					area->toBuildQueue_.pop_front();
					area->getFSM()->ChangeState(&GridAreaStartToBuild);
				}
			}
		}
	}
}