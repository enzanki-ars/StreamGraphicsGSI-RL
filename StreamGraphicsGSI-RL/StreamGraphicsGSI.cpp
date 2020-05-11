#include "HTTPRequest.hpp"
#include "StreamGraphicsGSI.h"

#include <sstream>
#include <iomanip>

BAKKESMOD_PLUGIN(StreamGraphicsGSI, "StreamGraphicsGSI", "1.3.0", PLUGINTYPE_THREADED)

using placeholders::_1;

const float COLOR_DARKEN = 0.75;

#pragma region Plugin Methods
void StreamGraphicsGSI::onLoad()
{
	cvarManager->registerCvar("streamgraphics_gsi_url", "http://192.168.1.70:8000/api/v1/set_multiple_for_path_and_flatten/data/com/rocketleague/gsi", "HTTP server url to send the JSON post to.", true, false, 0, false, 0, true);

	this->gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", std::bind(&StreamGraphicsGSI::ReplayStartEvent, this, _1));
	this->gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", std::bind(&StreamGraphicsGSI::ReplayEndEvent, this, _1));

	this->StartLoop();
}

void StreamGraphicsGSI::onUnload()
{
	ok = false;
}
#pragma endregion

#pragma region StreamGraphics
void StreamGraphicsGSI::StartLoop() {
	if (ok) {
		gameWrapper->Execute(std::bind(&StreamGraphicsGSI::UpdateMatchState, this));
		string newJson = GameState.GetJson().dump();
		if (Json != newJson) {
			Json = newJson;
			SendToStreamGraphics(newJson);
		}
		this->gameWrapper->SetTimeout(std::bind(&StreamGraphicsGSI::StartLoop, this), 0.100);
	}
}

void StreamGraphicsGSI::SendToStreamGraphics(std::string data)
{
	try {
		http::Request request(cvarManager->getCvar("streamgraphics_gsi_url").getStringValue());
		(void)request.send("POST", data, { "Content-Type: application/json" });
	}
	catch (...) {
		cvarManager->log("Failed GSI HTTP POST: Please reload the plugin to try again. Using URL - " + cvarManager->getCvar("streamgraphics_gsi_url").getStringValue());
		ok = false;
	}
}
#pragma endregion

#pragma region Events
void StreamGraphicsGSI::ReplayStartEvent(string eventName) {
	GameState.InReplayMode = true;
	cvarManager->log("Replay Started");
}

void StreamGraphicsGSI::ReplayEndEvent(string eventName) {
	GameState.InReplayMode = false;
	cvarManager->log("Replay Ended");
}

#pragma endregion

#pragma region Update State
void StreamGraphicsGSI::UpdateMatchState() {
	ServerWrapper wrapper = GetCurrentGameType();//this sets type (-1 for menu)

	if (!wrapper.IsNull())
		this->UpdateState(wrapper);
	else
		this->ResetStates();

	return;
}


void StreamGraphicsGSI::UpdateState(ServerWrapper wrapper) {

	if (!wrapper.GetbUnlimitedTime() && !wrapper.GetbOverTime()) {
		GameState.Match.Time = wrapper.GetSecondsRemaining();
		GameState.Match.Overtime = 0;

		std::string minutes = std::to_string(wrapper.GetSecondsRemaining() / 60);

		std::stringstream seconds_stream;
		seconds_stream << std::setw(2) << std::setfill('0') << std::to_string(wrapper.GetSecondsRemaining() % 60);

		std::string seconds = seconds_stream.str();

		GameState.Match.TimeString = minutes + ":" + seconds;
	}
	else if (!wrapper.GetbUnlimitedTime() && wrapper.GetbOverTime()) {
		GameState.Match.Time = (int) wrapper.GetOvertimeTimePlayed();
		GameState.Match.Overtime = 1;

		std::string minutes = std::to_string(((int) wrapper.GetOvertimeTimePlayed()) / 60);

		std::stringstream seconds_stream;
		seconds_stream << std::setw(2) << std::setfill('0') << std::to_string(((int) wrapper.GetOvertimeTimePlayed()) % 60);

		std::string seconds = seconds_stream.str();

		GameState.Match.TimeString = "+" + minutes + ":" + seconds;
	}
	else {
		GameState.Match.Time = -1;

		GameState.Match.TimeString = "Unlimited";
	}

	ArrayWrapper<TeamWrapper> teams = wrapper.GetTeams();
	for (int i = 0; i < teams.Count(); i++) {
		if (teams.Get(i).IsNull()) continue;

		GameState.Match.Teams[i].Goals = teams.Get(i).GetScore();
		GameState.Match.Teams[i].Index = teams.Get(i).GetTeamIndex();

		if (!teams.Get(i).GetSanitizedTeamName().IsNull())
			GameState.Match.Teams[i].Name = teams.Get(i).GetSanitizedTeamName().ToString();
		else
			GameState.Match.Teams[i].Name = i == 0 ? "Blue" : "Orange";

		LinearColor asd = teams.Get(i).GetFontColor();
		GameState.Match.Teams[i].Red = asd.R;
		GameState.Match.Teams[i].Green = asd.G;
		GameState.Match.Teams[i].Blue = asd.B;

		std::stringstream team_hex;
		team_hex << "#" << std::setw(6) << std::setfill('0') << std::hex << ((int)(asd.R * 0xff) << 16 | (int)(asd.G * 0xff) << 8 | (int)(asd.B * 0xff));
		GameState.Match.Teams[i].ColorHex = team_hex.str();

		std::stringstream team_hex_dark;
		team_hex_dark << "#" << std::setw(6) << std::setfill('0') << std::hex << ((int)(asd.R * COLOR_DARKEN * 0xff) << 16 | (int)(asd.G * COLOR_DARKEN * 0xff) << 8 | (int)(asd.B * COLOR_DARKEN * 0xff) & 0xff);
		GameState.Match.Teams[i].ColorDarkHex = team_hex_dark.str();
	}


	auto players = wrapper.GetPRIs();
	for (int i = 0; i < players.Count(); i++) {
		auto local = players.Get(i);
		if (!local.IsNull()) {
			if (local.GetUniqueId().ID == this->gameWrapper->GetSteamID()) {
				GameState.Player.Score = local.GetMatchScore();
				GameState.Player.Goals = local.GetMatchGoals();
				GameState.Player.Assists = local.GetMatchAssists();
				GameState.Player.Saves = local.GetMatchSaves();
				GameState.Player.Shots = local.GetMatchShots();

				GameState.Player.OwnGoals = local.GetMatchOwnGoals();
				GameState.Player.Demolishes = local.GetMatchDemolishes();

				GameState.Player.SpecSlot = local.GetSpectatorShortcut();

				GameState.Player.Name = local.GetPlayerName().ToString();
				GameState.Player.Ping = (int)local.GetExactPing();

				if (!local.GetTeam().IsNull() && (local.GetTeam().GetTeamIndex() == 0 || local.GetTeam().GetTeamIndex() == 1)) {
					GameState.Player.Team = local.GetTeam().GetTeamIndex();

					LinearColor asd = wrapper.GetTeams().Get(local.GetTeam().GetTeamIndex()).GetFontColor();

					std::stringstream team_hex;
					team_hex << "#" << std::setw(6) << std::setfill('0') << std::hex << ((int)(asd.R * 0xff) << 16 | (int)(asd.G * 0xff) << 8 | (int)(asd.B * 0xff));
					GameState.Player.ColorHex = team_hex.str();

					std::stringstream team_hex_dark;
					team_hex_dark << "#" << std::setw(6) << std::setfill('0') << std::hex << ((int)(asd.R * COLOR_DARKEN * 0xff) << 16 | (int)(asd.G * COLOR_DARKEN * 0xff) << 8 | (int)(asd.B * COLOR_DARKEN * 0xff) & 0xff);
					GameState.Player.ColorDarkHex = team_hex_dark.str();
				}
				else {
					GameState.Player.Team = -1;
					GameState.Player.ColorHex = "#1A1A1A";
					GameState.Player.ColorDarkHex = "#000000";
				}

				if (!local.GetCar().IsNull() && !local.GetCar().GetBoostComponent().IsNull())
					GameState.Player.CurrentBoostAmount = local.GetCar().GetBoostComponent().GetCurrentBoostAmount();
				else
					GameState.Player.CurrentBoostAmount = 0;
			}
		}
		else {
			GameState.Player.Team = 0;
			GameState.Player.Name = "";
			GameState.Player.Ping = 0;
			GameState.Player.ColorHex = "#1A1A1A";
			GameState.Player.ColorDarkHex = "#000000";

			GameState.Player.Score = 0;
			GameState.Player.Goals = 0;
			GameState.Player.Assists = 0;
			GameState.Player.Saves = 0;
			GameState.Player.Shots = 0;

			GameState.Player.OwnGoals = 0;
			GameState.Player.Demolishes = 0;

			GameState.Player.SpecSlot = 0;

			GameState.Player.CurrentBoostAmount = 0;
		}
	}

	GameSettingPlaylistWrapper playlistWrapper = wrapper.GetPlaylist();
	if (playlistWrapper.memory_address != NULL)
		GameState.Match.Playlist = playlistWrapper.GetPlaylistId();
	else
		GameState.Match.Playlist = -1;

	if (!gameWrapper->GetCamera().IsNull()) {
		PriWrapper spec_player = PriWrapper(reinterpret_cast<std::uintptr_t>(gameWrapper->GetCamera().GetViewTarget().PRI));
		if (!spec_player.IsNull() && !spec_player.GetCar().IsNull()) {
			std::string playerID = std::to_string(spec_player.GetUniqueId().ID);
			int team = spec_player.GetTeamNum();
			std::string name = spec_player.GetPlayerName().ToString();

			int ping = spec_player.GetExactPing();

			int score = spec_player.GetMatchScore();
			int goals = spec_player.GetMatchGoals();
			int assists = spec_player.GetMatchAssists();
			int saves = spec_player.GetMatchSaves();
			int shots = spec_player.GetMatchShots();

			int ownGoals = spec_player.GetMatchOwnGoals();
			int demolishes = spec_player.GetMatchDemolishes();

			int specSlot = spec_player.GetSpectatorShortcut();

			float boost = 0;

			if (!spec_player.GetCar().IsNull() && !spec_player.GetCar().GetBoostComponent().IsNull())
			{
				boost = spec_player.GetCar().GetBoostComponent().GetCurrentBoostAmount();
			}

			if (!spec_player.GetTeam().IsNull() && (team == 0 || team == 1)) {
				LinearColor asd = wrapper.GetTeams().Get(team).GetFontColor();

				std::stringstream team_hex;
				team_hex << "#" << std::setw(6) << std::setfill('0') << std::hex << ((int)(asd.R * 0xff) << 16 | (int)(asd.G * 0xff) << 8 | (int)(asd.B * 0xff));
				GameState.CurrentSpec.ColorHex = team_hex.str();

				std::stringstream team_hex_dark;
				team_hex_dark << "#" << std::setw(6) << std::setfill('0') << std::hex << ((int)(asd.R * COLOR_DARKEN * 0xff) << 16 | (int)(asd.G * COLOR_DARKEN * 0xff) << 8 | (int)(asd.B * COLOR_DARKEN * 0xff) & 0xff);
				GameState.CurrentSpec.ColorDarkHex = team_hex_dark.str();
			}
			else {
				GameState.CurrentSpec.ColorHex = "#1A1A1A";
				GameState.CurrentSpec.ColorDarkHex = "#000000";
			}

			GameState.CurrentSpec.Team = team;
			GameState.CurrentSpec.Name = name;
			GameState.CurrentSpec.Ping = ping;

			GameState.CurrentSpec.Score = score;
			GameState.CurrentSpec.Goals = goals;
			GameState.CurrentSpec.Assists = assists;
			GameState.CurrentSpec.Saves = saves;
			GameState.CurrentSpec.Shots = shots;

			GameState.CurrentSpec.OwnGoals = ownGoals;
			GameState.CurrentSpec.Demolishes = demolishes;

			GameState.CurrentSpec.SpecSlot = specSlot;

			GameState.CurrentSpec.CurrentBoostAmount = boost;

			GameState.IsSpectatingPlayerPOV = true;
		}
		else {
			GameState.CurrentSpec.Team = -1;
			GameState.CurrentSpec.Name = "";
			GameState.CurrentSpec.Ping = 0;
			GameState.CurrentSpec.ColorHex = "#1A1A1A";
			GameState.CurrentSpec.ColorDarkHex = "#000000";

			GameState.CurrentSpec.Score = 0;
			GameState.CurrentSpec.Goals = 0;
			GameState.CurrentSpec.Assists = 0;
			GameState.CurrentSpec.Saves = 0;
			GameState.CurrentSpec.Shots = 0;

			GameState.CurrentSpec.OwnGoals = 0;
			GameState.CurrentSpec.Demolishes = 0;

			GameState.CurrentSpec.SpecSlot = 0;

			GameState.CurrentSpec.CurrentBoostAmount = 0;

			GameState.IsSpectatingPlayerPOV = false;
		}
	}

	GameState.Match.Teams[0].TeamBoost = 0;
	GameState.Match.Teams[1].TeamBoost = 0;
	GameState.Match.Teams[0].PlayerCount = 0;
	GameState.Match.Teams[1].PlayerCount = 0;

	for (int i = 0; i < players.Count(); i++) {
		PriWrapper player = players.Get(i);

		std::string playerID = std::to_string(player.GetUniqueId().ID);
		int team = player.GetTeamNum();
		std::string name = player.GetPlayerName().ToString();

		int ping = player.GetExactPing();

		int score = player.GetMatchScore();
		int goals = player.GetMatchGoals();
		int assists = player.GetMatchAssists();
		int saves = player.GetMatchSaves();
		int shots = player.GetMatchShots();

		int ownGoals = player.GetMatchOwnGoals();
		int demolishes = player.GetMatchDemolishes();

		int specSlot = player.GetSpectatorShortcut();

		float boost = 0;

		if (!player.GetCar().IsNull() && !player.GetCar().GetBoostComponent().IsNull())
		{
			boost = player.GetCar().GetBoostComponent().GetCurrentBoostAmount();
		}

		if (specSlot >= 1 && specSlot <= 8 && (team == 0 || team == 1)) {
			GameState.SpecPlayers[specSlot - 1].Team = team;
			GameState.SpecPlayers[specSlot - 1].Name = name;
			GameState.SpecPlayers[specSlot - 1].Ping = ping;

			GameState.SpecPlayers[specSlot - 1].Score = score;
			GameState.SpecPlayers[specSlot - 1].Goals = goals;
			GameState.SpecPlayers[specSlot - 1].Assists = assists;
			GameState.SpecPlayers[specSlot - 1].Saves = saves;
			GameState.SpecPlayers[specSlot - 1].Shots = shots;

			GameState.SpecPlayers[specSlot - 1].OwnGoals = ownGoals;
			GameState.SpecPlayers[specSlot - 1].Demolishes = demolishes;

			GameState.SpecPlayers[specSlot - 1].SpecSlot = specSlot;

			GameState.SpecPlayers[specSlot - 1].CurrentBoostAmount = boost;

			GameState.Match.Teams[team].TeamBoost += boost;
			GameState.Match.Teams[team].PlayerCount += 1;

			if (!player.GetTeam().IsNull()) {
				LinearColor asd = wrapper.GetTeams().Get(team).GetFontColor();

				std::stringstream team_hex;
				team_hex << "#" << std::setw(6) << std::setfill('0') << std::hex << ((int)(asd.R * 0xff) << 16 | (int)(asd.G * 0xff) << 8 | (int)(asd.B * 0xff));
				GameState.SpecPlayers[specSlot - 1].ColorHex = team_hex.str();

				std::stringstream team_hex_dark;
				team_hex_dark << "#" << std::setw(6) << std::setfill('0') << std::hex << ((int)(asd.R * COLOR_DARKEN * 0xff) << 16 | (int)(asd.G * COLOR_DARKEN * 0xff) << 8 | (int)(asd.B * COLOR_DARKEN * 0xff) & 0xff);
				GameState.SpecPlayers[specSlot - 1].ColorDarkHex = team_hex_dark.str();
			}
			else {
				GameState.SpecPlayers[specSlot - 1].ColorHex = "#1A1A1A";
				GameState.SpecPlayers[specSlot - 1].ColorHex = "#000000";
			}
		}
	}
}


void StreamGraphicsGSI::ResetStates()
{
	GameState.Status = GameStatus::Menu;

	GameState.Match.Playlist = -1;
	GameState.Match.Time = 0;
	GameState.Match.Overtime = FALSE;
	GameState.Match.TimeString = "";

	GameState.Match.Teams[0].Index = 0;
	GameState.Match.Teams[0].PlayerCount = 0;
	GameState.Match.Teams[0].Goals = 0;
	GameState.Match.Teams[0].Red = 0;
	GameState.Match.Teams[0].Green = 0;
	GameState.Match.Teams[0].Blue = 0;
	GameState.Match.Teams[0].TeamBoost = 0.0;
	GameState.Match.Teams[0].Name = "";
	GameState.Match.Teams[0].ColorHex = "#1A1A1A";
	GameState.Match.Teams[1].ColorDarkHex = "#000000";

	GameState.Match.Teams[1].Index = 1;
	GameState.Match.Teams[1].PlayerCount = 0;
	GameState.Match.Teams[1].Goals = 0;
	GameState.Match.Teams[1].Red = 0;
	GameState.Match.Teams[1].Green = 0;
	GameState.Match.Teams[1].Blue = 0;
	GameState.Match.Teams[1].TeamBoost = 0.0;
	GameState.Match.Teams[1].Name = "";
	GameState.Match.Teams[1].ColorHex = "#1A1A1A";
	GameState.Match.Teams[1].ColorDarkHex = "#000000";

	GameState.Player.Team = -1;
	GameState.Player.Name = "";
	GameState.Player.Ping = 0;
	GameState.Player.ColorHex = "#1A1A1A";
	GameState.Player.ColorDarkHex = "#000000";

	GameState.Player.Assists = 0;
	GameState.Player.Goals = 0;
	GameState.Player.Saves = 0;
	GameState.Player.Score = 0;
	GameState.Player.Shots = 0;

	GameState.Player.OwnGoals = 0;
	GameState.Player.Demolishes = 0;

	GameState.Player.SpecSlot = 0;

	GameState.Player.CurrentBoostAmount = 0;


	GameState.CurrentSpec.Team = -1;
	GameState.CurrentSpec.Name = "";
	GameState.CurrentSpec.Ping = 0;
	GameState.CurrentSpec.ColorHex = "#1A1A1A";
	GameState.CurrentSpec.ColorDarkHex = "#000000";

	GameState.CurrentSpec.Score = 0;
	GameState.CurrentSpec.Goals = 0;
	GameState.CurrentSpec.Assists = 0;
	GameState.CurrentSpec.Saves = 0;
	GameState.CurrentSpec.Shots = 0;

	GameState.CurrentSpec.OwnGoals = 0;
	GameState.CurrentSpec.Demolishes = 0;

	GameState.CurrentSpec.SpecSlot = 0;

	GameState.CurrentSpec.CurrentBoostAmount = 0;

	GameState.IsSpectatingPlayerPOV = false;
	GameState.InReplayMode = false;

	for (int i = 0; i < 8; i++) {
		GameState.SpecPlayers[i].Team = -1;
		GameState.SpecPlayers[i].Name = "";
		GameState.SpecPlayers[i].Ping = 0;
		GameState.SpecPlayers[i].ColorHex = "#1A1A1A";
		GameState.SpecPlayers[i].ColorDarkHex = "#000000";

		GameState.SpecPlayers[i].Assists = 0;
		GameState.SpecPlayers[i].Goals = 0;
		GameState.SpecPlayers[i].Saves = 0;
		GameState.SpecPlayers[i].Score = 0;
		GameState.SpecPlayers[i].Shots = 0;

		GameState.SpecPlayers[i].OwnGoals = 0;
		GameState.SpecPlayers[i].Demolishes = 0;

		GameState.SpecPlayers[i].SpecSlot = 0;

		GameState.SpecPlayers[i].CurrentBoostAmount = 0;
	}
}

ServerWrapper StreamGraphicsGSI::GetCurrentGameType() {
	if (this->gameWrapper->IsInOnlineGame()) {
		GameState.Status = GameStatus::InGame;
		return this->gameWrapper->GetOnlineGame();
	}
	else if (this->gameWrapper->IsSpectatingInOnlineGame()) {
		GameState.Status = GameStatus::Spectate;
		return this->gameWrapper->GetOnlineGame();
	}
	else if (this->gameWrapper->IsInReplay()) {
		GameState.Status = GameStatus::Replay;
		return this->gameWrapper->GetGameEventAsReplay();
	}
	else if (this->gameWrapper->IsInFreeplay()) {
		GameState.Status = GameStatus::Freeplay;
		return this->gameWrapper->GetGameEventAsServer();
	}
	else if (this->gameWrapper->IsInCustomTraining()) {
		GameState.Status = GameStatus::Training;
		return this->gameWrapper->GetGameEventAsServer();
	}
	else if (this->gameWrapper->IsInGame()) {
		GameState.Status = GameStatus::InGame;
		return this->gameWrapper->GetGameEventAsServer();
	}
	else {
		GameState.Status = GameStatus::Menu;
		return NULL;
	}
}
#pragma endregion