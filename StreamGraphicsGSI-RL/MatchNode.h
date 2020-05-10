#pragma once
#include "json.hpp"
#include "TeamNode.h"
#include "PlayerNode.h"

class MatchNode {
public:
	TeamNode Teams[2];
	int Time = 0;
	std::string TimeString = "";
	int Playlist = 0;
	bool Overtime = 0;

	nlohmann::json GetJson();
};