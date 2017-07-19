#include "ChokeGridArea.h"
#include "data/data.h"

namespace CgBot
{

	BWAPI::Position ChokeGridArea::findNextBuildPosition(BWAPI::UnitType t)
	{
		if (t == BWAPI::UnitTypes::Protoss_Assimilator) {
			logger << "Try to find location for refinery" << std::endl;
			return gasPosition_;
		}
		return GridArea::findNextBuildPosition(t);
	};

	void ChokeGridArea::updateChokeWeight() {

	}

	void ChokeGridArea::removeChokeOutSideGrids() {

	}

	bool ChokeGridArea::isPylonReady() {
		return true;
	}
}