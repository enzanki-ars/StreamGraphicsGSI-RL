#pragma once
#include "json.hpp"

class TeamNode {
public:
	int Index = 0;
	int Goals = 0;

	float Red = 0;
	float Green = 0;
	float Blue = 0;

	float TeamBoost = 0.0;

	std::string Name = "";

	nlohmann::json GetJson();
};