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
}