#include <string>
#include <sstream>
#include <vector>

#include "GridArea.h"
#include "common/StateMachine.h"
#include "common/UtilHelper.h"


namespace CgBot{

	bool GridArea::isPositionInArea(const BWAPI::Position& pos) const
	{

		if ((center_.x - range_ < pos.x  && pos.x < center_.x + range_) &&
			(center_.y - range_  <pos.y && pos.y < center_.y + range_)){
			return true;
		}
		return false;
	}

	void GridArea::createGrids() {
		for (int x = -range_; x < range_; x = x + gridSize_){
			for (int y = -range_; y < range_; y = y + gridSize_){
				BWAPI::Position bp = BWAPI::Position(center_.x + x, center_.y + y);
				grids_.push_back(Grid(bp, 0));
			}
		}
	}

	void GridArea::drawGridArea() const{
		for (auto v : grids_){
				if (v.weight_ < 0) { continue;  }
				BWAPI::Broodwar->drawBoxMap(v.position_, BWAPI::Position(v.position_.x + 32, v.position_.y + 32), BWAPI::Colors::Blue);
				BWAPI::Broodwar->drawTextMap(BWAPI::Position(v.position_.x + 5, v.position_.y + 5), "%d", v.weight_);
		}
	}

	std::deque<BWAPI::UnitType>* GridArea::loadStringToBuildQueue(const std::string& src){
		std::vector<std::string> units = CgBot::split(src, ",");
		for (auto s : units) {
			toBuildQueue_.push_back(UnitTypeMap::getInstance().getUnitType(s));
		}
		return &toBuildQueue_;
	}


	bool GridArea::build(BWAPI::TilePosition p, BWAPI::UnitType t) {
		return builder_->build(t, p);
	}
	
	void GridArea::updateWeightAfterBuild(BWAPI::Unit unit) {
		for (std::vector<Grid>::iterator it = grids_.begin(); it != grids_.end(); ++it){
			if ((*it).weight_ >= 0 &&
				!canBuildHereWithSpace(BWAPI::TilePosition((*it).position_), getProviderType(), 0)) {
				//  check if it' buildable with pylon?
				(*it).weight_ = -1;
			}
		}
	}


	Grid* GridArea::findLeastDistanceGrid(BWAPI::UnitType t, BWAPI::Position pos) {
		Grid* grid = NULL;
		int distance = 10000;
		int width = t.width();
		int height = t.height();

		for (std::vector<Grid>::iterator it = grids_.begin(); it != grids_.end(); ++it){
			if ((*it).weight_ >= 0
				&& canBuildHereWithSpace(BWAPI::TilePosition((*it).position_), t, 0)){

				BWAPI::Position bp((*it).position_);
				if (pos.x < (*it).position_.x + width) {
					bp.x = (*it).position_.x + width;
				}
				if (pos.y < (*it).position_.y + height) {
					bp.y = (*it).position_.y + height;
				}

				int d = (int)pos.getDistance(bp);
				if (d < distance) {
					distance = d;
					grid = &(*it);
				}
			}
		}
		return grid;
	}


	bool GridArea::isPylonReady() {
		return false;
	}

	bool GridArea::canBuildHereWithSpace(BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist)
	{
		//returns true if we can build this type of unit here with the specified amount of space.
		//space value is stored in this->buildDistance.

		//if we can't build here, we of course can't build here with space
		if (!BWAPI::Broodwar->canBuildHere(position, type))
			return false;

		int width = type.tileWidth();
		int height = type.tileHeight();

		//make sure we leave space for add-ons. These types of units can have addons:
		if (type.canBuildAddon())
		{
			width += 2;
		}
		BWAPI::TilePosition start(position - BWAPI::TilePosition(buildDist, buildDist));
		BWAPI::TilePosition end(position + BWAPI::TilePosition(width, height) + BWAPI::TilePosition(buildDist, buildDist));
		if (!start || !end)
			return false;

		for (int x = start.x; x < end.x; ++x)
			for (int y = start.y; y < end.y; ++y)
			{
				if (!type.isRefinery() && !buildable(BWAPI::TilePosition(x, y)))
					return false;
			}

		if (position.x > 3)
		{
			BWAPI::Position start2(BWAPI::TilePosition(start.x - 2, start.y));
			BWAPI::Position end2(BWAPI::TilePosition(start.x, end.y));
			if (!BWAPI::Broodwar->getUnitsInRectangle(start2.makeValid(), end2.makeValid(), !BWAPI::Filter::IsLifted && BWAPI::Filter::CanBuildAddon).empty())
				return false;
		}
		return true;
	}

	BWAPI::TilePosition GridArea::getBuildLocationNear(BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist)
	{
		//returns a valid build location near the specified tile position.
		//searches outward in a spiral.
		int length = 1;
		int j = 0;
		bool first = true;
		int dx = 0;
		int dy = 1;
		while (length < 10) //We'll ride the spiral to the end
		{
			//if we can build here, return this tile position
			if (position)
				if (canBuildHereWithSpace(position, type, buildDist))
					return position;

			//otherwise, move to another position
			position += BWAPI::TilePosition(dx, dy);
			//count how many steps we take in this direction
			j++;
			if (j == length) //if we've reached the end, its time to turn
			{
				//reset step counter
				j = 0;

				//Spiral out. Keep going.
				if (!first)
					length++; //increment step counter if needed

				//first=true for every other turn so we spiral out at the right rate
				first = !first;

				//turn counter clockwise 90 degrees:
				if (dx == 0)
				{
					dx = dy;
					dy = 0;
				}
				else
				{
					dy = -dx;
					dx = 0;
				}
			}
			//Spiral out. Keep going.
		}
		return BWAPI::TilePositions::None;
	}

	bool GridArea::buildable(BWAPI::TilePosition position)
	{
		//returns true if this tile is currently buildable, takes into account units on tile
		if (!BWAPI::Broodwar->isBuildable(position))
			return false;

		if (!BWAPI::Broodwar->getUnitsOnTile(position, BWAPI::Filter::IsBuilding && !BWAPI::Filter::IsLifted).empty())
			return false;

		return true;
	}

	bool GridArea::train() {
		// search the building to train
		BWAPI::UnitType t = toBuildQueue_.front();
		if (minerals_ < t.mineralPrice()) {
			return false;
		}
		for (auto b : unitQueue_) {
			if (b->getType().buildsWhat().count(t) != 0){
				if (b->isTraining()){
					return false;
				}
				else{
					b->train(t);
					return true;
				}
			}
		}
		logger << "No unit to build : " << t.getName() << std::endl;
		return false;
	}

	void GridArea::onCreate(BWAPI::Unit unit) {
		// once a building is created, for protoss, ready to build another
		logger << unit->getType().getName() << " is building!" << std::endl;
		// change weight since there is new building in this area
		if (unit->getType().isBuilding()) {
			updateWeightAfterBuild(unit);
		}
		
		builder_->gather(builder_->getClosestUnit(BWAPI::Filter::IsMineralField));
		
		// move unit from queue
		toBuildQueue_.pop_front();
		// push to ongoing
		ongoingUnitQueue_.push_back(unit);
		// reset resource quota
		resourceQuota_.resetQuota();
		fsm_->ChangeState(&GridAreaStartToBuild);
	}

	void GridArea::onComplete(BWAPI::Unit unit){
		logger << unit->getType().getName() << " is completed!" << std::endl;
		// check unittype, if it's provider, update weight
		if (unit->getType() == getProviderType()){
			//updateChokeDistanceWeight(unit->getPosition());
		}
		//remove from ongoing queue
		std::remove(ongoingUnitQueue_.begin(), ongoingUnitQueue_.end(), unit);

		if (unit->getType().isBuilding() ){
			unit->setRallyPoint(center_);
		}
		unitQueue_.push_back(unit); // insert into completed unit queue
	}

	void GridArea::onFrame() {
		fsm_->Update(); // run fsm
		drawGridArea();
	}
}