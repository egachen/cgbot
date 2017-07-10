#pragma once

#include "common/State.h"
#include "ResourceBase.h"

namespace CgBot
{
	class ResourceBase;

	// Init State, begning of state
	DECLARE_CLASS(BaseInit, ResourceBase)

	// when game start, try to make worker as soon as possible
	DECLARE_CLASS(MakeWorker, ResourceBase)

	// in this state, try to reach one science
	DECLARE_CLASS(RushScience, ResourceBase)

}