#pragma once

#include <cassert>
#include <BWTA.h>
#include <BWAPI.h>
#include <memory>

#include "common/StateMachine.h"
#include "gridarea/GridArea.h"
#include "message/MessageBroker.h"
#include "gridarea/ChokeGridArea.h"
#include "WeightedArea.h"

namespace CgBot 
{

	class ResourceBase   //base class for all resource repo
	{
	public:
		ResourceBase(MessageBroker* broker, SortedAreas& areas):
			reservedMinerals_(0),
			messageBroker_(broker),
			chokeArea_(NULL),
			shipArea_(NULL),
			sortedAreas_(areas)
		{
			fsm_ = new StateMachine<ResourceBase>(this, "ResourceBase");
		};

		// initilize parameters in baselocation
		virtual void initBase();
		void kickOffState();
		// get minerals counter
		int getMinerals() const { BWAPI::Broodwar->self()->minerals(); };
		void trainWorker() const;
		void trainZealot() const;
		int getCurrentMinerals() const { return BWAPI::Broodwar->self()->minerals(); };
		int getAllocatedMinerals();

		void buildArea(GridArea* area); // start to build an area

		// resource management
		void allocateResource();

		StateMachine<ResourceBase> * getFSM() { return fsm_; };

		// main function to update each frame
		void onFrame();
		void onCreate(BWAPI::Unit unit);
		void onComplete(BWAPI::Unit unit);

		bool isZergEnemy() const;

		virtual ~ResourceBase() { delete fsm_;  };

	private:

		SortedAreas& sortedAreas_;

		std::vector<std::unique_ptr<GridArea>> areas_;
		int reservedMinerals_;

		StateMachine<ResourceBase> *fsm_;

		BWAPI::Position findNearestGas(BWAPI::Position pos);

	public:
		ChokeGridArea* chokeArea_;
		GridArea* shipArea_;

		Grids grids_;

		MessageBroker* messageBroker_;
		const unsigned int MaxWorkerNum = 18; // max number for worker in this base

	};
}