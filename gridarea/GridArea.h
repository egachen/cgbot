#pragma once

#include <map>

#include <BWAPI.h>
#include <cassert>
#include <queue>

#include "common/common.h"
#include "Grid.h"
#include "common/StateMachine.h"
#include "GridAreaState.h"
#include "data/data.h"

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


	typedef std::vector<Grid> Grids;
	// Hold a set of GridArea
	class GridArea
	{
	public:
		explicit GridArea(BWTA::Region* region, BWAPI::Position pos, std::string buildstr, std::string name);

		bool isPositionInArea(const BWAPI::Position& pos) const; // check if pos inside this area
		std::deque<BWAPI::UnitType>* loadStringToBuildQueue(const std::string& src); // convert string to build queue

		void createGrids(BWTA::Polygon* polygon);
	    Grids& getGrids()  { return grids_; };

		void updateWeightAfterBuild(BWAPI::Unit unit);
		Grid* findLeastDistanceGrid(BWAPI::UnitType t, BWAPI::Position pos);
		virtual BWAPI::Position findNextBuildPosition(BWAPI::UnitType t);

		void drawGridArea() const;  // draw grid map with weight
		bool build(BWAPI::TilePosition p, BWAPI::UnitType t); // get unit from queue and build it
		void onFrame();  // this is the main function of GridArea
		virtual void onCreate(BWAPI::Unit unit);
	    virtual void onComplete(BWAPI::Unit unit);
		void onMessage(); // TODO: message from other units in CgBot

		void assignWorker(BWAPI::Unit worker) { workers_.push_back(worker); };
		const std::vector<BWAPI::Unit>& getWorkers() const { return  workers_; };
		BWAPI::Unit getBuilder();
		void moveBuilderToCenter() { getBuilder()->move(getCenter()); };

		bool canBuildHereWithSpace(BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist);
		BWAPI::TilePosition getBuildLocationNear(BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist);
		bool buildable(BWAPI::TilePosition position);

		StateMachine<GridArea>* getFSM() const { return fsm_; };
		//start build state, overload by subclass
		void startBuild();
		virtual void setBuildingState() { fsm_->ChangeState(&GridAreaBuilding); };

		void buildPylonOnTrigger(); // build pylon on trigger, no enough pylon
		BWAPI::Position getCenter() const { return center_; };
		virtual bool isPylonReady(BWAPI::UnitType t, BWAPI::Position pos);

		// update self weight
		void updateAreaWeight() { weight_ = toBuildQueue_.size();  };

		int getWeight() const { return weight_; };

		virtual ~GridArea() { delete fsm_; std::vector<Grid>().swap(grids_); };

	public:
		ResourceQuota resourceQuota_;

		std::deque<BWAPI::UnitType> toBuildQueue_;  // the queue for the units to be built
		std::vector<BWAPI::Unit> unitQueue_; // the queue for the completed units in this area

	private:

		std::vector<BWAPI::Unit> workers_;
		BWAPI::Unit currentBuilder_;

		int weight_; // weight for each grid
		std::string name_;

		BWAPI::Position center_;
		BWTA::Region* region_;

		StateMachine<GridArea> *fsm_;
		BWAPI::UnitType providerType_;

		Grids grids_;

		bool ongoingSupply_;

	};


}