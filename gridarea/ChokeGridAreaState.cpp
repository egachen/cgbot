#include "ChokeGridAreaState.h"
#include "ChokeGridArea.h"

namespace CgBot
{

	DEFINE_OBJECT(ChokeBuilding, GridArea)
	void ChokeBuilding::Execute(GridArea* area) {
		BWAPI::Error err = BWAPI::Broodwar->getLastError();

		if (err != BWAPI::Errors::None){
			logger << "last error: " << BWAPI::Broodwar->getLastError() << std::endl;
		}
		
		if (area->resourceQuota_.isAllocated()) {
			if (area->getBuilder()->isIdle()) {
				if (area->toBuildQueue_.front().isRefinery()){
					// since there is no oncreate event for refinery
					area->getBuilder()->gather(area->getBuilder()->getClosestUnit(BWAPI::Filter::IsMineralField));
					area->resourceQuota_.resetQuota();
					area->toBuildQueue_.pop_front();
					area->getFSM()->ChangeState(&GridAreaStartToBuild);
				}
				else{
					// if can not build
					Grid* grid = area->findLeastDistanceGrid(area->toBuildQueue_.front(), area->getCenter());
					if (grid != NULL) {
						grid->weight_ = -1;
						area->getFSM()->ChangeState(&GridAreaStartToBuild);
					}
				}

			}
		}
	}
}