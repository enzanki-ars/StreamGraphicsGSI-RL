#pragma once
#include "json.hpp"

class ProviderNode {
public:
	std::string Name = "RocketLeague";
	int AppId = 13377331;

	nlohmann::json GetJson();
};