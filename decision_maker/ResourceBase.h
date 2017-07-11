#pragma once

#include <cassert>
#include <BWTA.h>
#include <BWAPI.h>
#include <memory>

#include "common/StateMachine.h"
#include "gridarea/GridArea.h"
#include "message/MessageBroker.h"
#include "gridarea/ChokeGridArea.h"
#include "gridarea/GasGridArea.h"
#include "WeightedArea.h"

namespace CgBot 
{
	class ResourceBase   //base class for all resource repo
	{
	public:
		ResourceBase(MessageBroker* broker, SortedAreas& areas):
			region_(NULL),
			choke_(NULL),
			baseLocation_(NULL),
			base_(NULL),
			reservedMinerals_(0),
			messageBroker_(broker),
			chokeArea_(NULL),
			gasArea_(NULL),
			shipArea_(NULL),
			allocatedGasWorker_(false),
			sortedAreas_(areas)
		{
			fsm_ = new StateMachine<ResourceBase>(this, "ResourceBase");
		};

		// initilize parameters in baselocation
		virtual void initBase();
		void kickOffState();
		// get minerals counter
		int getMinerals() const { base_->getPlayer()->minerals(); };
		void trainWorker() const;
		int getCurrentMinerals() const { return base_->getPlayer()->minerals(); };
		int getAllocatedMinerals();

		void buildArea(GridArea* area); // start to build an area

		// resource management
		void allocateResource();

		//isgasready , perhaps it should send a message by gas area and handle by resource base
		bool isGasReady() {
			return gasArea_->isGasReady();
		}
		bool hasGasWorker(){
			return allocatedGasWorker_;
		}

		void assignGasWorker();

		StateMachine<ResourceBase> * getFSM() { return fsm_; };

		// main function to update each frame
		void onFrame();
		void onCreate(BWAPI::Unit unit);
		void onComplete(BWAPI::Unit unit);

		virtual ~ResourceBase() { delete fsm_;  };

	private:
		BWTA::Region* region_;  // suppose BWTA will manage this memory
		BWTA::Chokepoint* choke_;
		BWTA::BaseLocation* baseLocation_;

		BWAPI::Unit base_;
		BWAPI::Race race_;

		SortedAreas& sortedAreas_;

		std::vector<std::unique_ptr<GridArea>> areas_;
		int reservedMinerals_;

		StateMachine<ResourceBase> *fsm_;

		bool allocatedGasWorker_;

		BWAPI::Position findNearestGas(BWAPI::Position pos);

	public:
		ChokeGridArea* chokeArea_;
		GasGridArea* gasArea_;
		GridArea* shipArea_;

		MessageBroker* messageBroker_;
		std::queue<BWAPI::Unit> worker_;
		const unsigned int MaxWorkerNum = 20; // max number for worker in this base

	};
}