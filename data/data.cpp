#include "data.h"

namespace CgBot
{
	CompletedUnits gCompletedUnits;  // init global instance for completeunits
	MyInfo gPlayerInfo;
	Enemies gEnemies;

	void CompletedUnits::addUnit(BWAPI::Unit unit) {
		units_[unit->getType()].push_back(unit);
	}

	bool CompletedUnits::isRequireBuildingReady(const BWAPI::UnitType t) const {
		for (std::map<const BWAPI::UnitType, int>::const_iterator i = t.requiredUnits().begin();
			i != t.requiredUnits().end(); i++) {
			if (i->first.isBuilding()
				&& i->first != t.getRace().getCenter()
				&& !isUnitTypeReady(i->first)) {
				//logger << t.getName() << " require: " << i->first.getName() << " is not ready" << std::endl;
				return false;
			}
		}
		return true;
	}

	bool CompletedUnits::isUnitTypeReady(const BWAPI::UnitType t) const {
		return units_.find(t) != units_.end();
	}

	void CompletedUnits::initCompletedUnits() {
		for (auto &u : BWAPI::Broodwar->self()->getUnits()) {
				gCompletedUnits.addUnit(u); // add default building into completedunits
		}
	}


	void PlayerInfo::initPlayerInfo(BWAPI::Player player){
		player_ = player;  // assign player pointer
		baseLocation_ = BWTA::getStartLocation(player);
		if (baseLocation_ == NULL)
			return;
		
		region_ = baseLocation_->getRegion();

		polygon_ = const_cast<BWTA::Polygon*>(&(region_->getPolygon()));

		// update playerinfo
		race_ = BWAPI::Broodwar->self()->getRace();
		providerType_ = race_.getSupplyProvider();

		// assign the closest choke point
		std::set<BWTA::Chokepoint*> chokepoints = region_->getChokepoints();
		double min_length = 10000;

		// iterate through all chokepoints and look for the one with the smallest gap (least width)
		for (std::set<BWTA::Chokepoint*>::iterator c = chokepoints.begin(); c != chokepoints.end(); c++){
			double length = (*c)->getWidth();
			if (length<min_length || choke_ == NULL){
				min_length = length;
				choke_ = *c;
			}
		}

		BWAPI::Broodwar << "inited player " << std::endl;
	}

	BWAPI::Position MyInfo::findNearestGas() const {
		int distance = 1000000;
		BWAPI::Position result;
		for (const auto& geyser : getBaseLocation()->getGeysers()) {
			BWAPI::Position p = BWAPI::Position(geyser->getInitialTilePosition());
			int d = int(p.getDistance(getRegion()->getCenter()));
			if (distance > d){
				distance = d;
				result = p;
			}

		}
		return result;
	}

	void MyInfo::initMyInfo() {
		PlayerInfo::initPlayerInfo(BWAPI::Broodwar->self());
		base_ = getFirstResourceDepot();
	}

	BWAPI::Unit MyInfo::getFirstResourceDepot() const {
		for (auto &u : BWAPI::Broodwar->self()->getUnits()) {
			if (u->getType().isResourceDepot()) {
				return u;
			}
		}
		return NULL;
	}

	void Enemies::initEnemies() {
		// Iterate all the players in the game using a std:: iterator
		BWAPI::Playerset players = BWAPI::Broodwar->getPlayers();
		for (auto p : players)
		{
			if (!p->isObserver())
				enemies_.emplace_back(p);
		}
	}
}