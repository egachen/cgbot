#include "ChokeGridArea.h"

namespace CgBot
{

	void ChokeGridArea::updateChokeWeight() {
		int id = GridArea::getRegionID(getBuilder()->getPosition());
		for (std::vector<Grid>::iterator it = GridArea::getGrids()->begin(); it != GridArea::getGrids()->end(); ++it){
			if (BWAPI::Broodwar->getRegionAt((*it).position_)->getID() >= id - 1 &&
				BWAPI::Broodwar->getRegionAt((*it).position_)->getID() <= id + 1){
				(*it).weight_ = 1;
			}
		}
	}

	void ChokeGridArea::removeChokeOutSideGrids() {
		for (std::vector<Grid>::iterator it = GridArea::getGrids()->begin(); it != GridArea::getGrids()->end(); ++it){
			if ((*it).weight_ == 0){
				(*it).weight_ = -1;
			}
		}
	}

	bool ChokeGridArea::isPylonReady() {
		// in gas area, if it's to build provider and refinery, return true
		if (toBuildQueue_.front() == getProviderType()){
			return true;
		}
		// check if there is pylon on this area, if there is, change state
		for (auto u : unitQueue_){
			if (u->getType() == getProviderType()){
				return true;
			}
		}
		return false;
	}

}