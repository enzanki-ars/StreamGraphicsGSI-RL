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
enum class MatchMode
{
	Undefined = -1,
	CasualDuel,
	CasualDoubles,
	CasualStandard,
	CasualChaos,
	Private,
	RankedDuel,
	RankedDoubles,
	RankedSoloStandard,
	RankedStandard,
	MutatorMashup,
	Tournament,
	RankedHoops,
	RankedRumble,
	RankedDropshot,
	RankedSnowday
};

class MatchNode {
public:
	MatchType Type = MatchType::Menu;
	MatchMode Mode = MatchMode::Undefined;
	TeamNode Teams[2];
	int Time;

	nlohmann::json GetJson();
};