#pragma once

#include "GridAreaState.h"

namespace CgBot
{
	// declaration of choke grid
	class GridArea;

	// In this state,  start to update choke weight
	DECLARE_CLASS(BuilderFirstInGrid, GridArea)

	DECLARE_CLASS(ReadyToBuild, GridArea)

	// builder is building constructure, will leave this state once oncreate
	DECLARE_CLASS(ChokeBuilding, GridArea)

}