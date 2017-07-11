#include "ResourceBase.h"
#include "ResourceBaseState.h"
#include "message/MessageBroker.h"
#include "log/log.h"
#include "gridarea/ChokeGridAreaState.h"

namespace CgBot 
{
	void ResourceBase::kickOffState(){
		fsm_->SetCurrentState(&ResourceBaseBaseInit);
	}

	void ResourceBase::initBase()
	{
		logger << "Init base now...." << std::endl;

		baseLocation_ = BWTA::getStartLocation(BWAPI::Broodwar->self());

		if (baseLocation_ != NULL)
			region_ = baseLocation_->getRegion();
		
		// assign the closest choke point
		std::set<BWTA::Chokepoint*> chokepoints = region_->getChokepoints();
		double min_length = 10000;

		BWAPI::Broodwar << "Star to analyze choke points" << std::endl;
		// iterate through all chokepoints and look for the one with the smallest gap (least width)
		for (std::set<BWTA::Chokepoint*>::iterator c = chokepoints.begin(); c != chokepoints.end(); c++){
			logger << "Choke center: " << (*c)->getCenter() << std::endl;
			double length = (*c)->getWidth();
			if (length<min_length || choke_ == NULL){
				min_length = length;
				choke_ = *c;
			}
		}
		
		BWAPI::Broodwar << "Choke center: " << choke_->getCenter() << std::endl;
		// get resource depot
		
		for (auto &u : BWAPI::Broodwar->self()->getUnits()) {
			if (u->getType().isResourceDepot()) {
				base_ = u;
				race_ = u->getType().getRace();
			}		
		}

		// init GridArea
		chokeArea_ = new ChokeGridArea(
			240, 32, choke_->getCenter(),
			"pylon,forge,gateway,cannon,cannon", "ChokeArea");
		std::unique_ptr<GridArea> area = std::unique_ptr<GridArea>(chokeArea_);
		sortedAreas_.addArea(area.get());
		areas_.push_back(std::move(area));

		BWAPI::Position gasPosition = findNearestGas(baseLocation_->getPosition());
		gasArea_ = new GasGridArea(
			200, 32, gasPosition,
			"gas,pylon,cybernetics,pylon,stargate", "GasArea");
		area = std::unique_ptr<GridArea>(gasArea_);
		sortedAreas_.addArea(area.get());
		areas_.push_back(std::move(area));

		BWAPI::Broodwar << "Woker to collect minerals" << std::endl;
		// make worker to collect minerals
		for (auto &u : BWAPI::Broodwar->self()->getUnits()) {
			if (u->getType().isWorker()) {
				worker_.push(u);
				u->gather(u->getClosestUnit(BWAPI::Filter::IsMineralField));
			}
		}

	}

	void ResourceBase::trainWorker() const {
		if (!base_->isTraining() && getCurrentMinerals() >= 50) {
			base_->train(race_.getWorker());
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
		auto builder = worker_.front();
		worker_.pop();
		area->assignBuilder(builder);
		area->startBuild();
	}

	void ResourceBase::onFrame() {
		fsm_->Update();  // run state machine of base

		// run state machine of gridarea
		for (auto &area : areas_) {
			area.get()->onFrame();
		}
	}

	void ResourceBase::onCreate(BWAPI::Unit unit){
		logger << unit->getType().getName() << " is building" << std::endl;
		// refinery has not on create event!!!!!!
		for (auto &area : areas_){
			if (area->isPositionInArea(unit->getPosition())){
				area->onCreate(unit);
			}
		}
	}

	void ResourceBase::onComplete(BWAPI::Unit unit){
		logger << unit->getType().getName() << " is completed" << std::endl;
		for (auto &area : areas_){
			if (area->isPositionInArea(unit->getPosition())){
				area->onComplete(unit);
				return;
			}
		}
		//else it's event for base
		if (worker_.size() > 3 && unit->getType().isWorker()){
			worker_.push(unit);
			unit->gather(unit->getClosestUnit(BWAPI::Filter::IsMineralField));
		}

	}

	void ResourceBase::assignGasWorker(){
		if (!allocatedGasWorker_){
				for (int i : {1, 2, 3}){
					auto gaser = worker_.front();
					gaser->gather(gaser->getClosestUnit(BWAPI::Filter::IsRefinery));
					worker_.pop();
				}
			}
		allocatedGasWorker_ = true;
	}

	BWAPI::Position ResourceBase::findNearestGas(BWAPI::Position pos){
		int distance = 1000000;
		BWAPI::Position result;
		for (const auto& geyser : baseLocation_->getGeysers()) {
			BWAPI::Position p = BWAPI::Position(geyser->getInitialTilePosition());
			int d = int(p.getDistance(pos));
			if (distance > d){
				distance = d;
				result = p;
			}
			
		}
		return result;
	}
}