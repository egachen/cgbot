#include "map.h"

namespace CgBot
{
	MapInfo gMapinfo;

	const LocationInfo* MapInfo::getNotExploredLocation(BWAPI::Unit unit) const {
		LocationInfo* loc = NULL;
		int minDistance = 10000;
		for (auto& location : map_) {
			if (location.getStatus() == LocationStatus::Init ){
				int distance = (int)unit->getPosition().getDistance(location.getLocationPostion());
				if (minDistance > distance) {
					minDistance = distance;
					*loc = location;
				}
			}
		}
		return loc;
	}

	void MapInfo::drawMap() const {
		//we will iterate through all the base locations, and draw their outlines.
		for (const auto& baseLocation : BWTA::getBaseLocations()) {
			BWAPI::TilePosition p = baseLocation->getTilePosition();

			//draw outline of center location
			BWAPI::Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
			BWAPI::Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
			BWAPI::Broodwar->drawBoxMap(leftTop, rightBottom, BWAPI::Colors::Blue);

			//draw a circle at each mineral patch
			for (const auto& mineral : baseLocation->getStaticMinerals()) {
				BWAPI::Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, BWAPI::Colors::Cyan);
			}

			//draw the outlines of Vespene geysers
			for (const auto& geyser : baseLocation->getGeysers()) {
				BWAPI::TilePosition p1 = geyser->getInitialTilePosition();
				BWAPI::Position leftTop1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
				BWAPI::Position rightBottom1(leftTop1.x + 4 * TILE_SIZE, leftTop1.y + 2 * TILE_SIZE);
				BWAPI::Broodwar->drawBoxMap(leftTop1, rightBottom1, BWAPI::Colors::Orange);
			}

			//if this is an island expansion, draw a yellow circle around the base location
			if (baseLocation->isIsland()) {
				BWAPI::Broodwar->drawCircleMap(baseLocation->getPosition(), 80, BWAPI::Colors::Yellow);
			}
		}

		//we will iterate through all the regions and ...
		for (const auto& region : BWTA::getRegions()) {
			// draw the polygon outline of it in green
			BWTA::Polygon p = region->getPolygon();
			for (size_t j = 0; j < p.size(); ++j) {
				BWAPI::Position point1 = p[j];
				BWAPI::Position point2 = p[(j + 1) % p.size()];
				BWAPI::Broodwar->drawLineMap(point1, point2, BWAPI::Colors::Green);
			}
			// visualize the chokepoints with red lines
			for (auto const& chokepoint : region->getChokepoints()) {
				BWAPI::Position point1 = chokepoint->getSides().first;
				BWAPI::Position point2 = chokepoint->getSides().second;
				BWAPI::Broodwar->drawLineMap(point1, point2, BWAPI::Colors::Red);
			}

		}
	}
}