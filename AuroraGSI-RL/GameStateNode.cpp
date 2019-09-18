#include "GameStateNode.h"

nlohmann::json GameStateNode::GetJson()
{
	nlohmann::json data;
	data["provider"]["name"] = "rocketleague";
	data["provider"]["appid"] = 13377331;
	data["game"]["status"] = Status;
	data["player"] = Player.GetJson();
	data["match"] = Match.GetJson();	

	return data;
}
