#pragma once
#include "json.hpp"
#include "TeamNode.h"
#include "PlayerNode.h"

class MatchNode {
public:
	TeamNode Teams[2];
	int Time;
	int Playlist;

	nlohmann::json GetJson();
};