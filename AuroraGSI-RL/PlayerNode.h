#pragma once
#include "json.hpp"

class PlayerNode {
public:
	int Team = -1;
	int Score = -1;
	int Goals = -1;
	int Assists = -1;
	int Saves = -1;
	int Shots = -1;
	float CurrentBoostAmount = -1;

	nlohmann::json GetJson();
};