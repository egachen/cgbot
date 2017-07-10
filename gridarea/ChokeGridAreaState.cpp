#include "ChokeGridAreaState.h"
#include "ChokeGridArea.h"

namespace CgBot
{
	DEFINE_OBJECT(BuilderFirstInGrid, GridArea)
	void BuilderFirstInGrid::Execute(GridArea* area){

		// in this state, always update chokeweight when distance to center < size*2
		if (area->builder_->getPosition().getDistance(area->getCenter()) > area->gridSize_ * 2){
			// down cast to choke grid area
			dynamic_cast<ChokeGridArea*>(area)->updateChokeWeight();
		}
		if (area->builder_->isIdle()) {  // if builder is idle, it means it reaches the center
			// change weight to -1 for all outside grid
			dynamic_cast<ChokeGridArea*>(area)->removeChokeOutSideGrids();
			area->getFSM()->ChangeState(&GridAreaStartToBuild);
		}
	}

	DEFINE_OBJECT(ReadyToBuild, GridArea)
	void ReadyToBuild::Execute(GridArea* area){
		assert(builder_ != NULL);

		area->moveBuilder(area->getCenter());

		if (area->isPositionInArea(area->builder_->getPosition())) {
			area->getFSM()->ChangeState(&GridAreaBuilderFirstInGrid);
		}
	}

	DEFINE_OBJECT(ChokeBuilding, GridArea)
	void ChokeBuilding::Execute(GridArea* area) {
		if (area->resourceQuota_.isAllocated()) {
			if (area->builder_->isIdle()) {
					Grid* grid = area->findLeastDistanceGrid(area->toBuildQueue_.front(), area->getCenter());
					if (grid != NULL) {
						grid->weight_ = -1;
						area->getFSM()->ChangeState(&GridAreaStartToBuild);
					}
			}
		}
	}
}