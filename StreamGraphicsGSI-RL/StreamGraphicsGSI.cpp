#include "HTTPRequest.hpp"
#include "StreamGraphicsGSI.h"

#include <sstream>

BAKKESMOD_PLUGIN(StreamGraphicsGSI, "StreamGraphicsGSI", "0.1.1", PLUGINTYPE_THREADED)

#pragma region Plugin Methods
void StreamGraphicsGSI::onLoad()
{
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
		http::Request request("http://localhost:9088");
		(void)request.send("POST", data, { "Content-Type: application/json" });
	}
	catch (...) {
		ok = false;
	}
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


void StreamGraphicsGSI::UpdateState(ServerWrapper wrapper)
{
	if (!wrapper.GetbUnlimitedTime() && !wrapper.GetbOverTime())
		GameState.Match.Time = wrapper.GetSecondsRemaining();
	else
		GameState.Match.Time = -1;

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

				if (!local.GetTeam().IsNull())
					GameState.Player.Team = local.GetTeam().GetTeamIndex();
				else
					GameState.Player.Team = 0;

				if (!local.GetCar().IsNull() && !local.GetCar().GetBoostComponent().IsNull())
					GameState.Player.CurrentBoostAmount = local.GetCar().GetBoostComponent().GetCurrentBoostAmount();
				else
					GameState.Player.CurrentBoostAmount = 0;
			}
		}
		else {
			GameState.Player.Team = 0;
			GameState.Player.Name = "";

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

	GameState.Match.Teams[0].TeamBoost = 0;
	GameState.Match.Teams[1].TeamBoost = 0;

	for (int i = 0; i < players.Count(); i++) {
		PriWrapper player = players.Get(i);

		std::string playerID = std::to_string(player.GetUniqueId().ID);
		int team = player.GetTeamNum();
		std::string name = player.GetPlayerName().ToString();

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
		}
	}
}


void StreamGraphicsGSI::ResetStates()
{
	GameState.Status = GameStatus::Menu;

	GameState.Match.Playlist = -1;
	GameState.Match.Time = 0;

	GameState.Match.Teams[0].Index = 0;
	GameState.Match.Teams[0].Goals = 0;
	GameState.Match.Teams[0].Red = 0;
	GameState.Match.Teams[0].Green = 0;
	GameState.Match.Teams[0].Blue = 0;
	GameState.Match.Teams[0].TeamBoost = 0.0;
	GameState.Match.Teams[0].Name = "";

	GameState.Match.Teams[1].Index = 0;
	GameState.Match.Teams[1].Goals = 0;
	GameState.Match.Teams[1].Red = 0;
	GameState.Match.Teams[1].Green = 0;
	GameState.Match.Teams[1].Blue = 0;
	GameState.Match.Teams[1].TeamBoost = 0.0;
	GameState.Match.Teams[1].Name = "";

	GameState.Player.Team = 0;
	GameState.Player.Name = "";

	GameState.Player.Assists = 0;
	GameState.Player.Goals = 0;
	GameState.Player.Saves = 0;
	GameState.Player.Score = 0;
	GameState.Player.Shots = 0;

	GameState.Player.OwnGoals = 0;
	GameState.Player.Demolishes = 0;

	GameState.Player.SpecSlot = 0;

	GameState.Player.CurrentBoostAmount = 0;

	for (int i = 0; i < 9; i++) {
		GameState.SpecPlayers[i].Team = 0;
		GameState.SpecPlayers[i].Name = "";

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