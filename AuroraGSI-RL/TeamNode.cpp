#include "TeamNode.h"

nlohmann::json TeamNode::GetJson()
{
	nlohmann::json json_team;
	//json_team["index"] = Index;
	json_team["goals"] = Goals;
	json_team["name"] = Name;
	json_team["color"]["red"] = Red;
	json_team["color"]["green"] = Green;
	json_team["color"]["blue"] = Blue;
	return json_team;
};
