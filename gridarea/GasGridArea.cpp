#include "GasGridArea.h"

namespace CgBot
{
	bool GasGridArea::isPylonReady() {
		// in gas area, if it's to build provider and refinery, return true
		if (toBuildQueue_.front() == getProviderType() ||
			toBuildQueue_.front().isRefinery()
			){
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

	Grid* GasGridArea::findNextBuildGrid(BWAPI::UnitType t) {
		BWAPI::Position pos = getCenter();
		// hard code for protoss now
		if (t == BWAPI::UnitTypes::Protoss_Assimilator){ // check if it's type is Assimilator, do not know exact function to check
			pos = gasPos_;
		}
		return findLeastDistanceGrid(t, pos); 
	};

	void GasGridArea::onComplete(BWAPI::Unit unit){
		GridArea::onComplete(unit);
		if (unit->getType().isRefinery()) { // if it's refinery
			for (auto gaser : worker_)
				gaser->gather(gaser->getClosestUnit(BWAPI::Filter::IsRefinery));

		}
	}
}