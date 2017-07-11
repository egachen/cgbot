#include "WeightedArea.h"

namespace CgBot
{
	void SortedAreas::sortAreas() {
		//update weight at first
		for (auto area : areas_) {
			area->updateAreaWeight();
		}

		std::sort(areas_.begin(), areas_.end(),
			[](const GridArea* a, const GridArea* b) -> bool
		{return a->getWeight() > b->getWeight();  });
	};

	void SortedAreas::allocateResource(int request, int current){
		for (auto &area : areas_){
			if (area->resourceQuota_.getStatus() == ResourceQuotaStatus::Requested){
				request = request + area->resourceQuota_.getUnitType().mineralPrice();
				if (current >= request){
					area->resourceQuota_.allocateQuota();
				}
			}
		}
	}
}