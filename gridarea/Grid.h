#pragma once
#include <BWAPI.h>

namespace CgBot
{
	// an area is split into m*n grids, the weight and height is from build move step for each building
	class Grid
	{
	public:
		explicit Grid(BWAPI::Position pos, int weight) :position_(pos), weight_(weight) {};
		bool operator==(const Grid& pos) const;

	public:
		int weight_; // each grid has a weight
		BWAPI::Position position_;
	};

}