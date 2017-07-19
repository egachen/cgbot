#include "GridAreaState.h"
#include "GridArea.h"
#include "decision_maker/ResourceBase.h"

namespace CgBot
{
	DEFINE_OBJECT(CreateGrid, GridArea)
	void CreateGrid::Execute(GridArea* area){
		// change state to idle
		area->getFSM()->ChangeState(&GridAreaIDLE);
	}

	DEFINE_OBJECT(IDLE, GridArea)
	void IDLE::Execute(GridArea* area){
	}

	DEFINE_OBJECT(StartToBuild, GridArea)
	void StartToBuild::Execute(GridArea* area){
		
		// check if there is element in queue
		if (area->toBuildQueue_.size() <= 0){
			area->getFSM()->ChangeState(&GridAreaCompleted);
			return;
		}

		BWAPI::UnitType t = area->toBuildQueue_.front();
		// if the required unit ready
		if (!gCompletedUnits.isRequireBuildingReady(t)){
			return;
		}
		if (!area->isPylonReady(t, area->getCenter())){
			return;
		}

		//request quota for this unit
		area->resourceQuota_.requestQuota(t);
		area->getFSM()->ChangeState(&GridAreaRequestQuota);
	}

	DEFINE_OBJECT(RequestQuota, GridArea)
	void RequestQuota::Execute(GridArea* area) {
		if (area->resourceQuota_.getStatus() == ResourceQuotaStatus::Allocated) {
			BWAPI::UnitType t = area->resourceQuota_.getUnitType();

			BWAPI::Position pos = area->findNextBuildPosition(t);
			if (pos == BWAPI::Position(0, 0)) {  // no grid to build? TODO: need check if there is problem
					area->getFSM()->ChangeState(&GridAreaBuildFailed);
					return;
			}
			
			logger << "To build " << t.getName() << " at " << pos << std::endl;

			area->build(BWAPI::TilePosition(pos), t);
			area->setBuildingState();// use virtual function to go to different building state
		}
	}

	DEFINE_OBJECT(TrainUnit, GridArea)
	void TrainUnit::Execute(GridArea* area) {
	}

	DEFINE_OBJECT(Building, GridArea)
	void Building::Execute(GridArea* area) {
		// TODO TODO TODO:
	
	}

	DEFINE_OBJECT(BuildFailed, GridArea)
	void BuildFailed::Execute(GridArea *area){
		// if failed to build
		/*
		logger << "Failed to build a constructure, do something, please" << std::endl;
		// sometimes, the can not find least weight
		area->updateChokeDistanceWeight(area->unitQueue_.front()->getPosition());
		area->getFSM()->ChangeState(&GridAreaStartToBuild);
		*/

	}

	DEFINE_OBJECT(Completed, GridArea)
	void Completed::Execute(GridArea* area){
		// remove builder?
	}

}