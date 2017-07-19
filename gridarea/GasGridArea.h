#pragma once

#include "GridArea.h"
#include "GasGridAreaState.h"

namespace CgBot
{
	class GasGridArea : public GridArea
	{
	public:
		GasGridArea(int range, int gridSize, BWAPI::Position pos, std::string buildstr, std::string name, BWAPI::Position gaspos) :
			GridArea(range, gridSize, pos, buildstr, name), gasPos_(gaspos)
		{
			logger << "Gasarea center: " << pos << "gas pos: " << gasPos_ << std::endl;
		};

		virtual void setBuildingState() { getFSM()->ChangeState(&GridAreaGasBuilding); };
		bool isPylonReady();
		virtual Grid* findNextBuildGrid(BWAPI::UnitType t);
		virtual void onComplete(BWAPI::Unit unit);
		void assignWorker(BWAPI::Unit t) { worker_.push_back(t); };

		virtual ~GasGridArea() {};

	private:
		std::vector<BWAPI::Unit> worker_;
		BWAPI::Position gasPos_;

	};

}