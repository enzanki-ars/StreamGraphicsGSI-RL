#include "PlayerNode.h"

nlohmann::json PlayerNode::GetJson()
{
	nlohmann::json json_player;

	json_player["team"] = Team;
	json_player["name"] = Name;

	json_player["score"] = Score;
	json_player["goals"] = Goals;
	json_player["assists"] = Assists;
	json_player["saves"] = Saves;
	json_player["shots"] = Shots;

	json_player["boost"] = CurrentBoostAmount;

	json_player["own_goals"] = OwnGoals;
	json_player["demos"] = Demolishes;

	json_player["spec_slot"] = SpecSlot;

	return json_player;
};
