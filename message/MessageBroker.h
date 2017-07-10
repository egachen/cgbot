#pragma once

#include <map>

#include <BWAPI.h>

#include "gridarea/GridArea.h"

namespace CgBot
{
	// this class is to be a broker between BWAPI and areas
	// area register it's range, once broker receive onCreate/onComplete
	// from BWAPI, it check the range, and pass the message to area
	class MessageBroker
	{
	public:
		MessageBroker() {};
		MessageBroker(const MessageBroker&) = delete; 
		MessageBroker& operator=(const MessageBroker&) = delete;

	public:
		// hold all grid area
		std::vector<GridArea*> gridAreaVector_;

	public:

		void onCreate(BWAPI::Unit unit){
			for (auto area : gridAreaVector_) {
				if (area->isPositionInArea(unit->getPosition())){
					area->onCreate(unit);
				}
			}
		}

		void onComplete(BWAPI::Unit unit){
			for (auto area : gridAreaVector_) {
				if (area->isPositionInArea(unit->getPosition())){
					area->onComplete(unit);
				}
			}
		}

		void insertArea(GridArea* area){
			gridAreaVector_.push_back(area);
		}

	};
}