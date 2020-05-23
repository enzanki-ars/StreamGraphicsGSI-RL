#include "TeamNode.h"
#include <algorithm>

nlohmann::json TeamNode::GetJson()
{
	nlohmann::json json_team;
	json_team["goals"] = Goals;
	json_team["saves"] = Saves;
	json_team["shots"] = Shots;
	json_team["demos"] = Demolishes;
	json_team["player_count"] = PlayerCount;
	json_team["name"] = Name;
	json_team["name_abvr"] = Name.substr(0, 1);

	json_team["red"] = Red;
	json_team["green"] = Green;
	json_team["blue"] = Blue;
	json_team["color_hex"] = ColorHex;
	json_team["color_css"] = "background-color: " + ColorHex + "; color: " + ColorTextHex + ";";
	json_team["color_dark_hex"] = ColorDarkHex;
	json_team["color_dark_css"] = "background-color: " + ColorDarkHex + "; color: " + ColorTextHex + ";";
	json_team["color_text_hex"] = ColorTextHex;

	json_team["boost"] = TeamBoost;

	if (PlayerCount > 0) {
		json_team["boost_average"] = TeamBoost / PlayerCount;
		json_team["boost_string"] = std::to_string(int(TeamBoost * 100)) + "%";
		json_team["boost_average_string"] = std::to_string(int((TeamBoost * 100) / PlayerCount)) + "%";
	}
	else {
		json_team["boost_average"] = 0;
		json_team["boost_string"] = "0%";
		json_team["boost_average_string"] = "0%";
	}

	return json_team;
};
