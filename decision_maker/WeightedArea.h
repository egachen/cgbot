#pragma once

#include "gridarea/GridArea.h"

namespace CgBot
{

	// class to hold containers for weighted area, and provide sorted method
	class SortedAreas
	{
	public:
		SortedAreas(){};

		void addArea(GridArea* area) { areas_.push_back(area); };
		void sortAreas();
		void allocateResource(int request, int current);

		~SortedAreas(){};

	private:
		std::vector<GridArea*> areas_;
	};

}