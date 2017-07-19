#pragma once

#include "common/bt.h"
#include "data/data.h"
#include "map/map.h"
#include "log/log.h"

namespace CgBot
{
	// check if scout is needed
	class IsTimeToScout : public Node
	{
	public:
		virtual bool run() override {
			// at least one cannon exist
			if (gCompletedUnits.getUnitCount(BWAPI::UnitTypes::Protoss_Photon_Cannon) == 0) {
				return false; // should not scout if defense is not ok
			}

			// loop enemies, check if it's explored
			for (auto e : gEnemies.getEnemies()) {
				if (e.ifNeedScout()) {
					return true;
				}
			}
			return false;
		}
	};

	// this class is the behavior to start scout
	class StartScout : public Node
	{
	private:
		BWAPI::Unit scouter_;

	public:
		StartScout(BWAPI::Unit scouter) : scouter_(scouter){};
		virtual bool run() override {
			// loop enemies, check if it's explored
			for (auto e : gEnemies.getEnemies()) {
				if (e.getBaseLocation() == NULL) {
					const LocationInfo* loc = gMapinfo.getNotExploredLocation(scouter_);
					if (loc == NULL) {
						logger << "no location to explore but this enemy base is still unknown" << std::endl;
						return false;
					}
					scouter_->move(loc->getLocationPostion());
					const_cast<LocationInfo*>(loc)->startToExplore();
					return true;
				}
				// if the enemy base is known, but has not scouted for long time
				scouter_->move(e.getBaseLocation()->getPosition());
				return true;
			}
			return false;
		}
	};
}