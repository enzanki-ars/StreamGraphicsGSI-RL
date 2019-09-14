#pragma once
#include "json.hpp"
#include "ProviderNode.h"
#include "MatchNode.h"
#include "PlayerNode.h"


class GameStateNode {
public:
	MatchNode Match;
	PlayerNode Player;

	nlohmann::json GetJson();
};