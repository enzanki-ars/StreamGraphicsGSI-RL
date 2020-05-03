#include "TeamNode.h"

nlohmann::json TeamNode::GetJson()
{
	nlohmann::json json_team;
	json_team["goals"] = Goals;
	json_team["name"] = Name;

	json_team["red"] = Red;
	json_team["green"] = Green;
	json_team["blue"] = Blue;

	json_team["boost"] = TeamBoost;
	json_team["boost_string"] = std::to_string(int((TeamBoost * 100)/4)) + "%";

	return json_team;
};
