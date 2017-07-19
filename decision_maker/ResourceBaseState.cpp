#include "ResourceBaseState.h"


namespace CgBot
{
	DEFINE_OBJECT(BaseInit, ResourceBase)
	void BaseInit::Execute(ResourceBase* rb) {
		rb->initBase();
		rb->getFSM()->ChangeState(&ResourceBaseMakeWorker);
	}

	DEFINE_OBJECT(MakeWorker, ResourceBase)
	void MakeWorker::Execute(ResourceBase* rb) {
		if (gCompletedUnits.getUnitCount(gPlayerInfo.getRace().getWorker()) == 7){
			rb->chokeArea_->moveBuilderToCenter();
		}
		if (gCompletedUnits.getUnitCount(gPlayerInfo.getRace().getWorker()) <= 7){
			rb->trainWorker();
		}
		else{
			rb->getFSM()->ChangeState(&ResourceBaseDefenseZergRush);
			rb->chokeArea_->startBuild();
		}
	}

	DEFINE_OBJECT(DefenseZergRush, ResourceBase)
	void DefenseZergRush::Execute(ResourceBase* rb) {
		if (gCompletedUnits.isUnitTypeReady(BWAPI::UnitTypes::Protoss_Photon_Cannon)){
			rb->getFSM()->ChangeState(&ResourceBaseRushScience);
		}
	}

	DEFINE_OBJECT(RushScience, ResourceBase)
	// in rush science state, try to defense choke and build science building
	void RushScience::Execute(ResourceBase* rb) {
		// TODO: check if there is reserved minerals for grids
		int m = rb->getCurrentMinerals() - rb->getAllocatedMinerals();
		if (m < 50){
			return;
		}
		if (gCompletedUnits.getUnitCount(gPlayerInfo.getRace().getWorker()) <= 18){
			rb->trainWorker();
		}
		if (gCompletedUnits.getUnitCount(BWAPI::UnitTypes::Protoss_Zealot) <= 4){
			rb->trainZealot();
		}
		
	}


}