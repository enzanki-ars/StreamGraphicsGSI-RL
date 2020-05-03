#include "MatchNode.h"

nlohmann::json MatchNode::GetJson()
{
	nlohmann::json json_match;
	json_match["team"]["0"] = Teams[0].GetJson();
	json_match["team"]["1"] = Teams[1].GetJson();
    json_match["playlist"] = Playlist;
	json_match["time"] = Time;
	return json_match;
};
