#pragma once
#include "json.hpp"
#include "ProviderNode.h"
#include "MatchNode.h"
#include "PlayerNode.h"

enum class GameStatus
{
	Menu = -1,
	Replay,
	InGame,
	Freeplay,
	Training,
	Spectate
};

class GameStateNode {
public:
	MatchNode Match;
	PlayerNode Player;
	PlayerNode CurrentSpec;
	PlayerNode SpecPlayers[8];
	GameStatus Status;

	bool IsSpectatingPlayerPOV = false;
	bool InReplayMode = false;

	nlohmann::json GetJson();
};