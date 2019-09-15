#include "TeamNode.h"

nlohmann::json TeamNode::GetJson()
{
	nlohmann::json json_team;
	//json_team["index"] = Index;
	json_team["goals"] = Goals;
	json_team["name"] = Name;
	json_team["red"] = Red;
	json_team["green"] = Green;
	json_team["blue"] = Blue;
	return json_team;
};
