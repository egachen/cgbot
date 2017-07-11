#pragma once

#include <map>

#include <BWAPI.h>
#include <cassert>
#include <queue>

#include "common/common.h"
#include "Grid.h"
#include "common/StateMachine.h"
#include "GridAreaState.h"

namespace CgBot
{
	// class for resource quota for trainning or building
	// will flag as consumed after used
	enum class ResourceQuotaStatus
	{
		Idle,   // default status
		Requested,   // if grid has requested
		Allocated   // if resource base has allocated
	};

	class ResourceQuota
	{
	public:
		ResourceQuota() :requestUnitType_(NULL),status_(ResourceQuotaStatus::Idle)
		{};

		void resetQuota(){ 
			status_ = ResourceQuotaStatus::Idle;
		};

		// grid area request to get resource quota
		void requestQuota(BWAPI::UnitType t){
			requestUnitType_ = t;
			status_ = ResourceQuotaStatus::Requested;
		}

		// resource manager to reserve quota
		void allocateQuota(){
			status_ = ResourceQuotaStatus::Allocated;
		}

		ResourceQuotaStatus getStatus(){
			return status_;
		};

		BWAPI::UnitType getUnitType(){
			return requestUnitType_;
		};

		bool isAllocated() {
			return status_ == ResourceQuotaStatus::Allocated;
		}

	private:
		BWAPI::UnitType requestUnitType_;
		ResourceQuotaStatus status_;

	};

	enum class AreaType {
		ChokeArea,
		BaseArea,
		GasArea
	};

	// Hold a set of GridArea
	class GridArea
	{
	public:
		GridArea(int range, int gridSize, BWAPI::Position pos, std::string buildstr, std::string name) :
			builder_(NULL),
			range_(range),
			gridSize_(gridSize),
			center_(pos),
			weight_(0)
		{
			loadStringToBuildQueue(buildstr);
			fsm_ = new StateMachine<GridArea>(this, name);
			fsm_->SetCurrentState(&GridAreaCreateGrid);
		};

		bool isPositionInArea(const BWAPI::Position& pos) const; // check if pos inside this area
		std::deque<BWAPI::UnitType>* loadStringToBuildQueue(const std::string& src); // convert string to build queue
		void createGrids(); // create grids in this area, centerpos is the center of this area

		void updateWeightAfterBuild(BWAPI::Unit unit);
		Grid* findLeastDistanceGrid(BWAPI::UnitType t, BWAPI::Position pos);
		virtual Grid* findNextBuildGrid(BWAPI::UnitType t){ return findLeastDistanceGrid(t, getCenter()); };

		void drawGridArea() const;  // draw grid map with weight
		bool build(BWAPI::TilePosition p, BWAPI::UnitType t); // get unit from queue and build it
		void onFrame();  // this is the main function of GridArea
		virtual void onCreate(BWAPI::Unit unit);
	    void onComplete(BWAPI::Unit unit);
		void onMessage(); // TODO: message from other units in CgBot

		void assignBuilder(BWAPI::Unit u) { builder_ = u; providerType_ = u->getType().getRace().getSupplyProvider(); };
		void moveBuilder(BWAPI::Position pos) const { // move builder to target area
			if (builder_->isMoving()){
				builder_->move(pos);
			}
		};
		bool train();
		bool canBuildHereWithSpace(BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist);
		BWAPI::TilePosition getBuildLocationNear(BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist);
		bool buildable(BWAPI::TilePosition position);

		StateMachine<GridArea>* getFSM() const { return fsm_; };
		//start build state, overload by subclass
		virtual void startBuild() { fsm_->ChangeState(&GridAreaStartToBuild); };
		virtual void setBuildingState() { fsm_->ChangeState(&GridAreaStartToBuild); };

		BWAPI::Position getCenter() const { return center_; };
		virtual bool isPylonReady();
		bool isGasReady() { return unitQueue_.size() == 1 && unitQueue_.front()->getType().isRefinery(); };
		// get supply provider type
		BWAPI::UnitType getProviderType(){ return providerType_; };

		// update self weight
		void updateAreaWeight() { weight_ = toBuildQueue_.size();  };

		// get the id of region by position
		int getRegionID(BWAPI::Position pos) { return BWAPI::Broodwar->getRegionAt(pos)->getID(); };
		int getWeight() const { return weight_; };
		int getGridSize() const { return gridSize_; };
		std::vector<Grid>* getGrids() { return &grids_; };
		BWAPI::Unit getBuilder() const { return builder_; };

		virtual ~GridArea() { delete fsm_; };

	public:
		ResourceQuota resourceQuota_;

		std::deque<BWAPI::UnitType> toBuildQueue_;  // the queue for the units to be built
		std::vector<BWAPI::Unit> unitQueue_; // the queue for the completed units in this area

	private:

		BWAPI::Unit builder_; //dedicate worker for this gridarea
		int range_;   // range for this area
		int gridSize_;   // size for each grid
		int minerals_;  // the resource for this area
		const int leastMineralsToBuild_ = 100;
		int weight_; // weight for each grid

		std::vector<BWAPI::Unit> ongoingUnitQueue_; // the queue for the ongoing units in this area

		std::vector<Grid> grids_;
		BWAPI::Position center_;

		StateMachine<GridArea> *fsm_;
		BWAPI::UnitType providerType_;

	};


}