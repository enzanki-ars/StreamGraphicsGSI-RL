#include "GameStateNode.h"

nlohmann::json GameStateNode::GetJson()
{
	nlohmann::json data;
	data["provider"]["name"] = "rocketleague";
	data["provider"]["appid"] = 13377331;
	data["game"]["status"] = Status;
	data["player"] = Player.GetJson();

	data["spec_players"]["1"] = SpecPlayers[0].GetJson();
	data["spec_players"]["2"] = SpecPlayers[1].GetJson();
	data["spec_players"]["3"] = SpecPlayers[2].GetJson();
	data["spec_players"]["4"] = SpecPlayers[3].GetJson();
	data["spec_players"]["5"] = SpecPlayers[4].GetJson();
	data["spec_players"]["6"] = SpecPlayers[5].GetJson();
	data["spec_players"]["7"] = SpecPlayers[6].GetJson();
	data["spec_players"]["8"] = SpecPlayers[7].GetJson();

	data["match"] = Match.GetJson();

	return data;
}
