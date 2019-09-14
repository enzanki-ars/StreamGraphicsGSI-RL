#include "MatchNode.h"

nlohmann::json MatchNode::GetJson()
{
	nlohmann::json json_match;
	//json_match["team_blue"] = Teams[0].GetJson();
	//json_match["team_orange"] = Teams[1].GetJson();
	json_match["teams"] = { Teams[0].GetJson() , Teams[1].GetJson() };
    //json_match["mode"] = Mode;
    json_match["type"] = Type;
	json_match["time"] = Time;
	return json_match;
};
