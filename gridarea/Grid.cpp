#include "Grid.h"

namespace CgBot{

	bool Grid::operator==(const Grid& pos) const
	{
		if (position_ == pos.position_ && weight_ == pos.weight_){
			return true;
		}
		return false;
	}
}