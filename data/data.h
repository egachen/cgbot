#pragma once

#include <BWAPI.h>
#include <BWTA.h>

namespace CgBot
{
	class CompletedUnits // save all completed unit
	{
	public:
		void addUnit(BWAPI::Unit unit);
		bool isRequireBuildingReady(const BWAPI::UnitType t) const;
		bool isUnitTypeReady(const BWAPI::UnitType t) const;
		std::vector<BWAPI::Unit>& getUnitVector(const BWAPI::UnitType t) { 
			return units_[t];
		};
		int getUnitCount(const BWAPI::UnitType t) {
			return isUnitTypeReady(t) ? units_[t].size() : 0;
		}
		void initCompletedUnits(); // call when game start

	private:
		std::map<BWAPI::UnitType, std::vector<BWAPI::Unit>> units_;

	public:
		int freeSupply_;
	};

	extern CompletedUnits gCompletedUnits;

	class PlayerInfo
	{
	public:
		explicit PlayerInfo() :
			region_(NULL), choke_(NULL), baseLocation_(NULL), 
			polygon_(NULL), race_(NULL), providerType_(NULL), player_(NULL){};
		explicit PlayerInfo(BWAPI::Player player) : 
			player_(player), 
			race_(player->getRace()),
			providerType_(player->getRace().getSupplyProvider()),
			region_(NULL), choke_(NULL), baseLocation_(NULL),
			polygon_(NULL) {};

		BWTA::Region* getRegion() const { return region_; };
		BWTA::Chokepoint* getChokePoints() const { return choke_; };
		BWTA::Polygon* getPolygon() const { return polygon_; };
		BWTA::BaseLocation* getBaseLocation() const { return baseLocation_; };
		BWAPI::Race getRace() const { return race_; };
		BWAPI::UnitType getProviderType() const { return providerType_; };
		BWAPI::Player getPlayer() const { return player_; };
		void initPlayerInfo(BWAPI::Player player);

	private:
		BWTA::Region* region_;  // suppose BWTA will manage this memory
		BWTA::Chokepoint* choke_;
		BWTA::BaseLocation* baseLocation_;
		BWTA::Polygon* polygon_;
		BWAPI::Race race_;
		BWAPI::UnitType providerType_;
		BWAPI::Player player_;
	};

	class MyInfo : public PlayerInfo
	{
	public:
		explicit MyInfo() : PlayerInfo(),
			base_(NULL){};
		void initMyInfo();
		BWAPI::Position findNearestGas() const;
		BWAPI::Unit getFirstResourceDepot() const;
		BWAPI::Unit getBaseUnit() const { return base_; };

	private:
		BWAPI::Unit base_;

	};

	extern MyInfo gPlayerInfo;

	class EnemyInfo : public PlayerInfo
	{
	public:
		EnemyInfo(BWAPI::Player player) : PlayerInfo(player), scoutframe_(0){};
		const bool ifNeedScout() const { 
			return getBaseLocation() == NULL || BWAPI::Broodwar->getFrameCount() - scoutframe_ > 100000;
		};

	private:
		int scoutframe_; // the frame when explored 
	};

	class Enemies{
	public:
		void initEnemies();
		const std::vector<EnemyInfo>& getEnemies() const { return enemies_; };

	private:
		std::vector<EnemyInfo> enemies_;
	};

	extern Enemies gEnemies;
}