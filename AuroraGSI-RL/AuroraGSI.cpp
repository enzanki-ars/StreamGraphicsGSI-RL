#include "HTTPRequest.hpp"
#include "AuroraGSI.h"

//using std::placeholders::_1;
BAKKESMOD_PLUGIN(AuroraGSI, "AuroraGSI", "0.1.0", PLUGINTYPE_THREADED)

#pragma region Plugin Methods
void AuroraGSI::onLoad()
{
	this->StartLoop();
}

void AuroraGSI::onUnload()
{
	ok = false;
	cvarManager->log("false");
}
#pragma endregion

#pragma region Aurora
void AuroraGSI::StartLoop() {
	while (ok) {
		gameWrapper->Execute(std::bind(&AuroraGSI::UpdateMatchState, this));
		string newJson = GameState.GetJson().dump();
		if (Json != newJson) {
			Json = newJson;
			SendToAurora(newJson);
		}	
		Sleep(100);
	}
	cvarManager->log("stop");
}

void AuroraGSI::SendToAurora(std::string data)
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
void AuroraGSI::UpdateMatchState() {
	ServerWrapper wrapper = GetCurrentGameType();//this sets type (-1 for menu)

	if (!wrapper.IsNull()) 
		this->UpdateState(wrapper);		
	else
		this->ResetStates();

	return;
}


void AuroraGSI::UpdateState(ServerWrapper wrapper)
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

				if (!local.GetTeam().IsNull())
					GameState.Player.Team = local.GetTeam().GetTeamIndex();
				else
					GameState.Player.Team = -1;

				if (!local.GetCar().IsNull() && !local.GetCar().GetBoostComponent().IsNull())
					GameState.Player.CurrentBoostAmount = local.GetCar().GetBoostComponent().GetCurrentBoostAmount();
				else
					GameState.Player.CurrentBoostAmount = -1;
			}
		}
		else {
			GameState.Player.Score = -1;
			GameState.Player.Goals = -1;
			GameState.Player.Assists = -1;
			GameState.Player.Saves = -1;
			GameState.Player.Shots = -1;
			GameState.Player.Team = -1;
			GameState.Player.CurrentBoostAmount = -1;
		}
	}

	GameSettingPlaylistWrapper playlistWrapper = wrapper.GetPlaylist();
	if (playlistWrapper.memory_address != NULL) 
		GameState.Match.Playlist = playlistWrapper.GetPlaylistId();
	else
		GameState.Match.Playlist = -1;
}


void AuroraGSI::ResetStates()
{
	GameState.Status = GameStatus::Menu;

	GameState.Match.Playlist = -1;
	GameState.Match.Time = -1;
	GameState.Match.Teams[0].Index = -1;
	GameState.Match.Teams[0].Goals = -1;
	GameState.Match.Teams[0].Red = -1;
	GameState.Match.Teams[0].Green = -1;
	GameState.Match.Teams[0].Blue = -1;
	GameState.Match.Teams[0].Name = "";
	GameState.Match.Teams[1].Index = -1;
	GameState.Match.Teams[1].Goals = -1;
	GameState.Match.Teams[1].Red = -1;
	GameState.Match.Teams[1].Green = -1;
	GameState.Match.Teams[1].Blue = -1;
	GameState.Match.Teams[1].Name = "";

	GameState.Player.Team = -1;
	GameState.Player.CurrentBoostAmount = -1;
	GameState.Player.Assists = -1;
	GameState.Player.Goals = -1;
	GameState.Player.Saves = -1;
	GameState.Player.Score = -1;
	GameState.Player.Shots = -1;
}

ServerWrapper AuroraGSI::GetCurrentGameType() {
	if (this->gameWrapper->IsInReplay()) {
		GameState.Status = GameStatus::Replay;
		return this->gameWrapper->GetGameEventAsReplay();
	}
	else if (this->gameWrapper->IsInOnlineGame()) {
		GameState.Status = GameStatus::OnlineGame;
		return this->gameWrapper->GetOnlineGame();
	}
	else if (this->gameWrapper->IsInFreeplay()) {
		GameState.Status = GameStatus::Freeplay;
		return this->gameWrapper->GetGameEventAsServer();
	}
	else if (this->gameWrapper->IsInCustomTraining()) {
		GameState.Status = GameStatus::Training;
		return this->gameWrapper->GetGameEventAsServer();
	}
	else if (this->gameWrapper->IsSpectatingInOnlineGame()) {
		GameState.Status = GameStatus::Spectate;
		return this->gameWrapper->GetOnlineGame();
	}
	else {
		GameState.Status = GameStatus::Menu;
		return NULL;
	}
}
#pragma endregion

