#pragma once

#include <BWAPI.h>
#include <BWTA.h>

namespace CgBot
{
	enum class LocationStatus 
	{
		Init,
		Exploring,
		Explored
	};

	class LocationInfo
	{
	public:
		explicit LocationInfo(BWTA::BaseLocation* location) : location_(location), status_(LocationStatus::Init){};
		const BWAPI::Position getLocationPostion() const { return location_->getPosition();  };
		const LocationStatus getStatus() const { return status_; };
		void startToExplore() { status_ = LocationStatus::Exploring; };

	private:
		BWTA::BaseLocation* location_;
		LocationStatus status_;
	};

	class MapInfo
	{
	public:
		void initMapInfo() {
			//we will iterate through all the base locations, and draw their outlines.
			for (const auto& baseLocation : BWTA::getBaseLocations()) {
				map_.emplace_back(baseLocation);
			}
		}

		const std::vector<LocationInfo>& getLocations() const { return map_; };
		const LocationInfo* getNotExploredLocation(BWAPI::Unit unit) const;

		void drawMap() const;
			
	private:
		std::vector<LocationInfo> map_;
	};

	extern MapInfo gMapinfo;
}