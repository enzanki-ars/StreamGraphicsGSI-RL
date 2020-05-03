#pragma once
#include "json.hpp"

class PlayerNode {
public:
	int Team = -1;
	std::string Name = "";

	int Score = 0;
	int Goals = 0;
	int Assists = 0;
	int Saves = 0;
	int Shots = 0;

	int OwnGoals = 0;
	int Demolishes = 0;

	int SpecSlot = 0;

	float CurrentBoostAmount = 0;

	nlohmann::json GetJson();
};