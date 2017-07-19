#include <string>
#include <sstream>
#include <vector>

#include "GridArea.h"
#include "common/StateMachine.h"
#include "common/UtilHelper.h"
#include "data/data.h"


namespace CgBot{

	GridArea::GridArea(BWTA::Region* region, BWAPI::Position pos, std::string buildstr, std::string name) :
		region_(region),
		weight_(0),
		name_(name),
		center_(pos),
		currentBuilder_(NULL),
		ongoingSupply_(false)
	{
		loadStringToBuildQueue(buildstr);
		fsm_ = new StateMachine<GridArea>(this, name);
		fsm_->SetCurrentState(&GridAreaCreateGrid);

	};

	void GridArea::createGrids(BWTA::Polygon* polygon) {
		// get max box
		int minx = 10000, miny = 10000, maxx = 0, maxy = 0;
		for (const auto& p1 : *polygon){
			if (p1.x < minx) {
				minx = p1.x;
			}
			if (p1.y < miny) {
				miny = p1.y;
			}
			if (p1.x > maxx){
				maxx = p1.x;
			}
			if (p1.y > maxy){
				maxy = p1.y;
			}
		}


		// create grids
		for (int x = minx; x < maxx; x = x + TILE_SIZE){
			if (x + TILE_SIZE > BWAPI::Broodwar->mapWidth()*TILE_SIZE){
				continue;
			}
			for (int y = miny; y < maxy; y = y + TILE_SIZE){
				if (y + TILE_SIZE > BWAPI::Broodwar->mapHeight()*TILE_SIZE){
					continue;
				}
				BWAPI::Position bp = BWAPI::Position(x, y);
				BWAPI::Position bp2 = BWAPI::Position(x, y + TILE_SIZE);
				BWAPI::Position bp3 = BWAPI::Position(x, y + TILE_SIZE);
				BWAPI::Position bp4 = BWAPI::Position(x, y + TILE_SIZE);

				int weight = 0;
				if (!polygon->isInside(bp)) {
					weight++;
				}
				if (!polygon->isInside(bp2)) {
					weight++;
				}
				if (!polygon->isInside(bp3)) {
					weight++;
				}
				if (!polygon->isInside(bp4)) {
					weight++;
				}

				if (weight == 4 ) {
					weight = -1;
				}

				if (weight != -1 && weight != 0) {
					if (x + TILE_SIZE >= BWAPI::Broodwar->mapWidth()*TILE_SIZE || x - TILE_SIZE <= 0){
						weight = 10;
					}
					if (y + TILE_SIZE >= BWAPI::Broodwar->mapHeight()*TILE_SIZE || y - TILE_SIZE <= 0){
						weight = 10;
					}
				}

				grids_.push_back(Grid(bp, weight));
			}
		}
	}

	bool GridArea::isPositionInArea(const BWAPI::Position& pos) const
	{
		return region_->getPolygon().isInside(pos);
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

	BWAPI::Unit GridArea::getBuilder() {
		if (currentBuilder_) {
			return currentBuilder_;
		}
		if (workers_.empty()){
			logger << "No avaliable worker to build" << std::endl;
			abort();
		}

		currentBuilder_ =  workers_.front();
		return currentBuilder_;
	}

	void GridArea::startBuild() {
		 fsm_->ChangeState(&GridAreaStartToBuild);
	}

	bool GridArea::build(BWAPI::TilePosition p, BWAPI::UnitType t) {
		getBuilder()->move(BWAPI::Position(p));
		return getBuilder()->build(t, p);
	}
	
	void GridArea::updateWeightAfterBuild(BWAPI::Unit unit) {
		for (Grids::iterator it = grids_.begin(); it != grids_.end(); it++){
			if ((*it).weight_ >= 0 &&
				!canBuildHereWithSpace(BWAPI::TilePosition((*it).position_), gPlayerInfo.getProviderType(), 0)) {
				//  check if it' buildable with pylon?
				//logger << u.position_ << " can not build" << std::endl;
				(*it).weight_ = -1;
			}
		}
	}

	BWAPI::Position GridArea::findNextBuildPosition(BWAPI::UnitType t)
	{ 

		Grid* grid = findLeastDistanceGrid(t, getCenter());
		if (grid == NULL) {  // no grid to build? TODO: need check if there is problem
			logger << "No Grid to build : " << t.getName() << std::endl;
			return BWAPI::Position(0,0);
		}
		return grid->position_;
	};

	Grid* GridArea::findLeastDistanceGrid(BWAPI::UnitType t, BWAPI::Position pos) {
		Grid* grid = NULL;
		int distance = 10000;
		int width = t.width();
		int height = t.height();

		for (Grids::iterator it = grids_.begin(); it != grids_.end(); it++){
			
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
					grid = &((*it));
				}
			}
		}
		return grid;
	}


	bool GridArea::isPylonReady(BWAPI::UnitType t, BWAPI::Position pos) {
		if (t == gPlayerInfo.getProviderType()){
			return true;
		}
		if (gCompletedUnits.getUnitCount(gPlayerInfo.getProviderType()) > 0){
			return true;
		}
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

	void GridArea::buildPylonOnTrigger() {
		if (gCompletedUnits.freeSupply_ < 2 && toBuildQueue_.front() != gPlayerInfo.getProviderType() && !ongoingSupply_) {
			toBuildQueue_.push_front(gPlayerInfo.getProviderType());
		}
	}


	void GridArea::onCreate(BWAPI::Unit unit) {
		if (unit->getType() != toBuildQueue_.front()){
			logger << name_ << ": unexpected unit on create event, expected: " << toBuildQueue_.front().getName()
				<< " received: " << unit->getType().getName() << std::endl;
			return;
		}
		// once a building is created, for protoss, ready to build another
		logger << name_ << " : " << unit->getType().getName() << " is building!" << std::endl;

		// move unit from queue
		toBuildQueue_.pop_front();

		// reset resource quota
		resourceQuota_.resetQuota();

		if (!unit->getType().isBuilding()) {
			return;  // if it's not building, return
		}
		if (unit->getType() == gPlayerInfo.getProviderType()) {
			ongoingSupply_ = true;
		}
		// change weight since there is new building in this area
		updateWeightAfterBuild(unit);
		buildPylonOnTrigger(); // check if more supply provider needed
		getBuilder()->returnCargo();
		fsm_->ChangeState(&GridAreaStartToBuild);
	}

	void GridArea::onComplete(BWAPI::Unit unit){
	
		logger << name_ << " : " << unit->getType().getName() << " is completed!" << std::endl;
		// check unittype, if it's provider, update weight


		if (unit->getType().isBuilding() ){
			unit->setRallyPoint(center_);
		}

		//if worker
		if (unit->getType() == gPlayerInfo.getRace().getWorker()){
			assignWorker(unit);
			unit->gather(unit->getClosestUnit(BWAPI::Filter::IsMineralField));
		}

		// if it's refinery
		if (unit->getType().isRefinery()) {
			for (std::vector<BWAPI::Unit>::iterator it = workers_.end()-1; it > workers_.end() - 4; it--){
				(*it)->gather((*it)->getClosestUnit(BWAPI::Filter::IsRefinery));
			}
		}
		// set flag for ongoing supply
		if (unit->getType() == gPlayerInfo.getProviderType()) {
			ongoingSupply_ = false;
		}
		// update center if pylon is ready
		if (unit->getType() == gPlayerInfo.getProviderType() &&
			gCompletedUnits.getUnitCount(gPlayerInfo.getProviderType()) > 1) {
			center_ = unit->getPosition();
		}
	}

	void GridArea::onFrame() {
		fsm_->Update(); // run fsm
		drawGridArea();
	}
}