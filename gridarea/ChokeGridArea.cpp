#include "ChokeGridArea.h"

namespace CgBot
{

	void ChokeGridArea::updateChokeWeight() {
		int id = GridArea::getRegionID(builder_->getPosition());
		for (std::vector<Grid>::iterator it = grids_.begin(); it != grids_.end(); ++it){
			if (BWAPI::Broodwar->getRegionAt((*it).position_)->getID() >= id - 1 &&
				BWAPI::Broodwar->getRegionAt((*it).position_)->getID() <= id + 1){
				(*it).weight_ = 1;
			}
		}
	}

	void ChokeGridArea::removeChokeOutSideGrids() {
		for (std::vector<Grid>::iterator it = grids_.begin(); it != grids_.end(); ++it){
			if ((*it).weight_ == 0){
				(*it).weight_ = -1;
			}
		}
	}



}