#pragma once

#include "GridArea.h"
#include "ChokeGridAreaState.h"

namespace CgBot
{
	class ChokeGridArea : public GridArea
	{
	public:
		ChokeGridArea(BWTA::Region* region, BWAPI::Position pos,
			std::string buildstr, std::string name, BWAPI::Position gasPos, BWAPI::Unit base) :
			GridArea(region, pos, buildstr, name), gasPosition_(gasPos), base_(base)
		{};

		BWAPI::Position findNextBuildPosition(BWAPI::UnitType t);
		void updateChokeWeight();
		void removeChokeOutSideGrids();  // set outside of choke grids to -1 weight

		virtual void setBuildingState() { getFSM()->ChangeState(&GridAreaChokeBuilding); };
		
		bool isPylonReady();

	private:
		BWAPI::Position gasPosition_;
		BWAPI::Unit  base_;

	};

}