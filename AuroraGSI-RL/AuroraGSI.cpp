#include "HTTPRequest.hpp"
#include "AuroraGSI.h"

//using std::placeholders::_1;
BAKKESMOD_PLUGIN(AuroraGSI, "AuroraGSI", "0.0.1", PLUGINTYPE_THREADED)

#pragma region Plugin Methods
void AuroraGSI::onLoad()
{
	//this->gameWrapper->HookEventPost("Function TAGame.PlayerController_TA.PostBeginPlay", std::bind(&AuroraGSI::HookMatchStarted, this, _1));
	//this->gameWrapper->HookEventPost("Function TAGame.Ball_TA.RecordCarHit", std::bind(&AuroraGSI::HookPlayerChanged, this, _1));
	//this->gameWrapper->HookEventPost("Function TAGame.GFxData_PRI_TA.HandleTeamChanged", std::bind(&AuroraGSI::HookPlayerChanged, this, _1));
	//this->gameWrapper->HookEventPost("Function TAGame.ArenaSoundManager_TA.HandlePlayerScored", std::bind(&AuroraGSI::HookGoalScored, this, _1));
	this->StartLoop();
}

void AuroraGSI::onUnload()
{
	//gameWrapper->UnhookEvent("Function TAGame.PlayerController_TA.PostBeginPlay");
	//gameWrapper->UnhookEvent("Function TAGame.GFxData_PRI_TA.HandleTeamChanged");
	//gameWrapper->UnhookEvent("Function TAGame.Ball_TA.RecordCarHit");
	//gameWrapper->UnhookEvent("Function TAGame.ArenaSoundManager_TA.HandlePlayerScored");
}
#pragma endregion

#pragma region Aurora
void AuroraGSI::StartLoop() {
	for (;;) {
		this->UpdateMatchState();
		SendToAurora(GameState.GetJson().dump());
		Sleep(100);
	}
}

void AuroraGSI::SendToAurora(std::string data)
{
	try {
		http::Request request("http://localhost:9088");
		(void)request.send("POST", data, { "Content-Type: application/json" });
	}
	catch (...) {
		//ignore
	}
}
#pragma endregion

#pragma region Update State
bool AuroraGSI::UpdateMatchState() {
	//cvarManager->log("UpdateMatchState");
	ServerWrapper wrapper = this->GetCurrentGameType();//this sets type (-1 for menu)

	if (!wrapper.IsNull()) 
		this->UpdateState(wrapper);	
	else
		this->ResetStates();

	return true;
}


void AuroraGSI::UpdateState(ServerWrapper wrapper)
{

	if(!wrapper.GetbUnlimitedTime() && !wrapper.GetbOverTime())
		GameState.Match.Time = wrapper.GetSecondsRemaining();

	auto teams = wrapper.GetTeams();
	for (int i = 0; i < teams.Count(); i++) {
		if (teams.Get(i).IsNull()) continue;

		GameState.Match.Teams[i].Goals = teams.Get(i).GetScore();
		GameState.Match.Teams[i].Index = teams.Get(i).GetTeamIndex();

		if (!teams.Get(i).GetSanitizedTeamName().IsNull())
			GameState.Match.Teams[i].Name = teams.Get(i).GetSanitizedTeamName().ToString();
		else
			GameState.Match.Teams[i].Name = i == 0 ? "Blue" : "Orange";

		auto asd = teams.Get(i).GetFontColor();
		GameState.Match.Teams[i].Red = (int)(asd.R * 255);
		GameState.Match.Teams[i].Green = (int)(asd.G * 255);
		GameState.Match.Teams[i].Blue = (int)(asd.B * 255);
	}

	auto players = wrapper.GetPRIs();
	if (players.Count() > 0) {
		auto local = players.Get(0);

		GameState.Player.Score = local.GetMatchScore();
		GameState.Player.Goals = local.GetMatchGoals();
		GameState.Player.Assists = local.GetMatchAssists();
		GameState.Player.Saves = local.GetMatchSaves();
		GameState.Player.Shots = local.GetMatchShots();

		if (!local.GetTeam().IsNull())
			GameState.Player.Team = local.GetTeam().GetTeamIndex();
		else
			GameState.Player.Team = -1;

		if (!local.GetCar().IsNull() && !local.GetCar().GetBoostComponent().IsNull()) {
			GameState.Player.CurrentBoostAmount = local.GetCar().GetBoostComponent().GetCurrentBoostAmount();
		}
	}

	//Playlist stuff not working for some reason, always returns garbage
	//try {
	//	auto mmrwrapper = gameWrapper->GetMMRWrapper();
	//	if (mmrwrapper.memory_address != NULL) {
	//		auto asd = mmrwrapper.GetCurrentPlaylist();
	//		cvarManager->log("mmr mode " +
	//			std::to_string(asd));
	//	}
	//	//GameSettingPlaylistWrapper playlistWrapper = wrapper.GetPlaylist();
	//	//if (playlistWrapper.memory_address != NULL) {
	//	//	GameState.Match.Mode = this->GetCurrentGameMode(playlistWrapper.GetPlaylistId());
	//	//	cvarManager->log("set mode " +
	//	//		std::to_string(playlistWrapper.GetPlaylistId()));
	//	//	//	std::to_string(playlistWrapper.IsValid2()));
	//	//}
	//	
	//}
	//catch (...) {};
	//cvarManager->log("4");
}


void AuroraGSI::ResetStates()
{
	GameState.Match.Mode = MatchMode::Undefined;
	GameState.Match.Type = MatchType::Menu;
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
	if (gameWrapper->IsInReplay()) {
		GameState.Match.Type = MatchType::Replay;
		return gameWrapper->GetGameEventAsReplay().memory_address;
	}
	else if (gameWrapper->IsInOnlineGame()) {
		GameState.Match.Type = MatchType::OnlineGame;
		return gameWrapper->GetOnlineGame();
	}
	else if (gameWrapper->IsInFreeplay()) {
		GameState.Match.Type = MatchType::Freeplay;
		return gameWrapper->GetGameEventAsServer();
	}
	else if (gameWrapper->IsInCustomTraining()) {
		GameState.Match.Type = MatchType::Training;
		return gameWrapper->GetGameEventAsServer();
	}
	else if (gameWrapper->IsSpectatingInOnlineGame()) {
		GameState.Match.Type = MatchType::Spectate;
		return gameWrapper->GetOnlineGame();
	}
	else {
		GameState.Match.Type = MatchType::Menu;
		return NULL;
	}
}

//Code mostly taken from https://github.com/segalll/DiscordRPCPlugin
MatchMode AuroraGSI::GetCurrentGameMode(int playlistId) {
	switch (playlistId) {
	case(1):
		return MatchMode::CasualDuel;
	case(2):
		return MatchMode::CasualDoubles;
	case(3):
		return MatchMode::CasualStandard;
	case(4):
		return MatchMode::CasualChaos;
	case(6):
		return MatchMode::Private;
	case(10):
		return MatchMode::RankedDuel;
	case(11):
		return MatchMode::RankedDoubles;
	case(12):
		return MatchMode::RankedSoloStandard;
	case(13):
		return MatchMode::RankedStandard;
	case(14):
		return MatchMode::MutatorMashup;
	case(22):
		return MatchMode::Tournament;
	case(27):
		return MatchMode::RankedHoops;
	case(28):
		return MatchMode::RankedRumble;
	case(29):
		return MatchMode::RankedDropshot;
	case(30):
		return MatchMode::RankedSnowday;
	default:
		return MatchMode::Undefined;
	}
}
#pragma endregion

