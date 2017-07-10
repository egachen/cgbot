#pragma once

#include "GridArea.h"
#include "ChokeGridAreaState.h"

namespace CgBot
{
	class ChokeGridArea : public GridArea
	{
	public:
		ChokeGridArea(int range, int gridSize, BWAPI::Position pos, std::string buildstr, std::string name) :
			GridArea(range, gridSize, pos, buildstr, name)
		{
			returnToWork_ = true;
		};

		void updateChokeWeight();
		void removeChokeOutSideGrids();  // set outside of choke grids to -1 weight
		
		// for choke grid, the state after create should be readytobuild, move builder to choke
		virtual void startBuild() { getFSM()->ChangeState(&GridAreaReadyToBuild); };
		virtual void setBuildingState() { getFSM()->ChangeState(&GridAreaChokeBuilding); };

		virtual ~ChokeGridArea() {};

	};

}