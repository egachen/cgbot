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
		rb->trainWorker();
		if (rb->worker_.size() == 7){
			// change gridarea state to readytobuild
			rb->buildArea(rb->chokeArea_);
			rb->getFSM()->ChangeState(&ResourceBaseRushScience);
		}
	}

	DEFINE_OBJECT(RushScience, ResourceBase)
	// in rush science state, try to defense choke and build science building
	void RushScience::Execute(ResourceBase* rb) {
		// alocated resource
		rb->allocateResource();

		if (rb->worker_.size() == 9){
			// change gridarea state to readytobuild
			rb->buildArea(rb->gasArea_);
		}

		// TODO: check if there is reserved minerals for grids
		int m = rb->getCurrentMinerals() - rb->getAllocatedMinerals();
		if (m < 50){
			return;
		}
		// in this state, keep train worker with highest priority
		if (rb->worker_.size() < rb->MaxWorkerNum){
			rb->trainWorker(); // TODO: in this function getCurrentMinerals might should be getMyallocatedMineranls
		}

		if (rb->isGasReady()){
			rb->assignGasWorker();
		}
	}


}