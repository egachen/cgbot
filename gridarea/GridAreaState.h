#pragma once

#include "common/State.h"

namespace CgBot {

	class GridArea;

	// The state class for creating grid for grid area
	// use singlton pattern here since there is no data persistent by state class
	DECLARE_CLASS(CreateGrid, GridArea)
	DECLARE_CLASS(IDLE, GridArea)

	// builder start to build a constructure
	DECLARE_CLASS(StartToBuild, GridArea)
	// request quota
	DECLARE_CLASS(RequestQuota, GridArea)

	// builder is building constructure, will leave this state once oncreate
	DECLARE_CLASS(Building, GridArea)
	// Failed to build a constructure
	DECLARE_CLASS(BuildFailed, GridArea)

	DECLARE_CLASS(TrainUnit, GridArea)

	DECLARE_CLASS(Completed, GridArea)

}
