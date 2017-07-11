#pragma once

#include "GridArea.h"
#include "GasGridAreaState.h"

namespace CgBot
{
	class GasGridArea : public GridArea
	{
	public:
		GasGridArea(int range, int gridSize, BWAPI::Position pos, std::string buildstr, std::string name) :
			GridArea(range, gridSize, pos, buildstr, name)
		{};

		virtual void setBuildingState() { getFSM()->ChangeState(&GridAreaGasBuilding); };
		bool isPylonReady();

		virtual ~GasGridArea() {};

	};

}