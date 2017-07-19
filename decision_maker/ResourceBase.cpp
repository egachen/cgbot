#include <stdlib.h> 

#include "ResourceBase.h"
#include "ResourceBaseState.h"
#include "message/MessageBroker.h"
#include "log/log.h"
#include "gridarea/ChokeGridAreaState.h"
#include "data/data.h"
#include "map/map.h"


namespace CgBot 
{

	void ResourceBase::kickOffState(){
		fsm_->SetCurrentState(&ResourceBaseBaseInit);
	}

	void ResourceBase::initBase()
	{
		logger << "init base..." << std::endl;
		// init GridArea
		chokeArea_ = new ChokeGridArea(
			gPlayerInfo.getRegion(),
			gPlayerInfo.getChokePoints()->getCenter(),
			"pylon,forge,cannon,gateway,gas,cannon,cannon,cannon,cannon,cannon,pylon,cybernetics,stargate,stargate,pylon", 
			"ChokeArea",
			gPlayerInfo.findNearestGas(),
			gPlayerInfo.getFirstResourceDepot()
			);
		chokeArea_->createGrids(gPlayerInfo.getPolygon());
		std::unique_ptr<GridArea> area = std::unique_ptr<GridArea>(chokeArea_);
		sortedAreas_.addArea(area.get());
		areas_.push_back(std::move(area));

		logger << "assign worker for base area" << std::endl;
		for (auto worker : gCompletedUnits.getUnitVector(gPlayerInfo.getRace().getWorker())) {
			chokeArea_->assignWorker(worker);
			worker->gather(worker->getClosestUnit(BWAPI::Filter::IsMineralField));
		}

		chokeArea_->updateWeightAfterBuild(gPlayerInfo.getBaseUnit());
	}

	void ResourceBase::trainWorker() const {
		if (!gPlayerInfo.getBaseUnit()->isTraining() && getCurrentMinerals() >= 50) {
			gPlayerInfo.getBaseUnit()->train(gPlayerInfo.getRace().getWorker());
		}
	}

	void ResourceBase::trainZealot() const {
		if (!gCompletedUnits.isUnitTypeReady(BWAPI::UnitTypes::Protoss_Gateway)) {
			return;
		}
		for (auto gateway : gCompletedUnits.getUnitVector(BWAPI::UnitTypes::Protoss_Gateway)){
			if (!gateway->isTraining()) {
				gateway->train(BWAPI::UnitTypes::Protoss_Zealot);
				return;

			}
		}
	}

	void ResourceBase::allocateResource(){
		sortedAreas_.allocateResource(getAllocatedMinerals(), getCurrentMinerals());
	}

	int ResourceBase::getAllocatedMinerals(){
		int m = 0;
		for (auto &area : areas_){
			if (area.get()->resourceQuota_.getStatus() == ResourceQuotaStatus::Allocated){
				m += area.get()->resourceQuota_.getUnitType().mineralPrice();
			}
		}
		return m;
	}

	void ResourceBase::buildArea(GridArea* area){

	}

	void ResourceBase::onFrame() {
		// alocated resource
		//allocateResource();
		fsm_->Update();  // run state machine of base
		allocateResource();
		gCompletedUnits.freeSupply_ = BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed();

		// run state machine of gridarea
		chokeArea_->onFrame();
	}

	void ResourceBase::onCreate(BWAPI::Unit unit){
		logger << unit->getType().getName() << " is building" << std::endl;
		if (unit->getPlayer() != BWAPI::Broodwar->self()){  // exclude units from others
			return;
		}

		// refinery has not on create event!!!!!!
		for (auto &area : areas_){
			if (area->isPositionInArea(unit->getPosition())){
				area->onCreate(unit);
			}
		}
	}

	void ResourceBase::onComplete(BWAPI::Unit unit){
		// TODO: if enemy is found
		if (unit->getPlayer() != BWAPI::Broodwar->self()){  // exclude units from others
			return;
		}

		logger << unit->getType().getName() << " is completed" << std::endl;
		gCompletedUnits.addUnit(unit);

		for (auto &area : areas_){
			if (area->isPositionInArea(unit->getPosition())){
				area->onComplete(unit);
				return;
			}
		}

	}

	bool ResourceBase::isZergEnemy() const {
		auto races = BWAPI::Broodwar->getPlayers().getRaces();
		return races.contains(BWAPI::Races::Zerg);
	}


}