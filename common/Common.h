#pragma once

#ifdef EXPORT_TEST_FUNCTIONS

#define MY_CPP_UNITTESTAPP_EXPORT __declspec(dllexport)
#else
#define MY_CPP_UNITTESTAPP_EXPORT
#endif

#include <BWAPI.h>

namespace CgBot
{
	class UnitTypeMap {
	public:
		static UnitTypeMap& getInstance()
		{
			static UnitTypeMap instance;
			return instance;
		}
	private:
		UnitTypeMap(){ InitUnitTypeMap(); };

	public:
		UnitTypeMap(UnitTypeMap const&) = delete;
		void operator=(UnitTypeMap const&) = delete;

	private:
		// can not make this map global and static, perhaps it's because BWAPI is on DLL mode
		std::map<std::string, BWAPI::UnitType> unitTypeMap_;

	public:
		void InitUnitTypeMap(){
			unitTypeMap_["pylon"] = BWAPI::UnitTypes::Protoss_Pylon;
			unitTypeMap_["forge"] = BWAPI::UnitTypes::Protoss_Forge;
			unitTypeMap_["cannon"] = BWAPI::UnitTypes::Protoss_Photon_Cannon;
			unitTypeMap_["gateway"] = BWAPI::UnitTypes::Protoss_Gateway;
			unitTypeMap_["zealot"] = BWAPI::UnitTypes::Protoss_Zealot;
			unitTypeMap_["gas"] = BWAPI::UnitTypes::Protoss_Assimilator;
			unitTypeMap_["cybernetics"] = BWAPI::UnitTypes::Protoss_Cybernetics_Core;
			unitTypeMap_["stargate"] = BWAPI::UnitTypes::Protoss_Stargate;
		};

		BWAPI::UnitType getUnitType(std::string key){
			return unitTypeMap_[key];
		}
	};
}
