#pragma once
#include "json.hpp"
#include "TeamNode.h"
#include "PlayerNode.h"

enum class MatchType
{
	Menu = -1,
	Replay,
	OnlineGame,
	Freeplay,
	Training,
	Spectate
};

class MatchNode {
public:
	MatchType Type;
	TeamNode Teams[2];
	int Time;
	int Mode;

	nlohmann::json GetJson();
};