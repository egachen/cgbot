
#include "..\GridArea.h"

#ifdef EXPORT_TEST_FUNCTIONS

#define MY_CPP_UNITTESTAPP_EXPORT __declspec(dllexport)
#else
#define MY_CPP_UNITTESTAPP_EXPORT
#endif

namespace CgBot
{
	MY_CPP_UNITTESTAPP_EXPORT bool testLoadStringToBuildQueue();

	bool testLoadStringToBuildQueue() {
		GridArea area(20, 10);
		bool result = area.loadStringToBuildQueue("pylon,forge");
		return result;
	}

}
