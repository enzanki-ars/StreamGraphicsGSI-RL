#include "PlayerNode.h"

nlohmann::json PlayerNode::GetJson()
{
	nlohmann::json json_player;

	json_player["team"] = Team;
	json_player["name"] = Name;
	json_player["ping"] = Ping;
	json_player["color_hex"] = ColorHex;
	json_player["color_css"] = "background-color: " + ColorHex + ";";
	json_player["color_dark_hex"] = ColorDarkHex;
	json_player["color_dark_css"] = "background-color: " + ColorDarkHex + ";";

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
