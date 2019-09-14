#pragma once
#include "json.hpp"

class TeamNode {
public:
	int Index = -1;
	int Goals = -1;
	int Red = -1;
	int Green = -1;
	int Blue = -1;
	std::string Name = "";

	nlohmann::json GetJson();
};