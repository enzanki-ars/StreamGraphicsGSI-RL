#pragma once
#include "json.hpp"

class TeamNode {
public:
	int Index = -1;
	int Goals = -1;
	float Red = -1;
	float Green = -1;
	float Blue = -1;
	std::string Name = "";

	nlohmann::json GetJson();
};