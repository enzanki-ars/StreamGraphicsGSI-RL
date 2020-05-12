#pragma once
#include "json.hpp"

class TeamNode {
public:
	int Index = 0;
	int PlayerCount = 0;
	int Goals = 0;
	int Saves = 0;
	int Shots = 0;
	int Demolishes = 0;

	float Red = 0;
	float Green = 0;
	float Blue = 0;
	std::string ColorHex = "#1A1A1A";
	std::string ColorDarkHex = "#000000";

	float TeamBoost = 0.0;

	std::string Name = "";

	nlohmann::json GetJson();
};