#pragma once
#include "json.hpp"

class PlayerNode {
public:
	int Team = -1;
	std::string Name = "";

	std::string ColorHex = "#1A1A1A";
	std::string ColorDarkHex = "#000000";
	std::string ColorTextHex = "#000000";

	int Ping = 0;

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