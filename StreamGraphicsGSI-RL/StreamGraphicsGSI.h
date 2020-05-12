#pragma once
#pragma comment( lib, "BakkesMod.lib" )
#include <bakkesmod/plugin/bakkesmodplugin.h>
#include "GameStateNode.h"


class StreamGraphicsGSI : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	virtual void onLoad();
	virtual void onUnload();
private:
#pragma region Fields
	GameStateNode GameState;
	std::string Json;
	bool ok = true;
#pragma endregion

#pragma region StreamGraphics
	void StartLoop();
	void SendToStreamGraphics(std::string);
	void ShowPlayerInfo();
	void HidePlayerInfo();
	void SendEvent(std::string event_url);
#pragma endregion

#pragma region Events
	void ReplayStartEvent(string eventName);
	void ReplayEndEvent(string eventName);
#pragma endregion

#pragma region Update States
	void UpdateMatchState();
	void UpdateState(ServerWrapper wrapper);
	void ResetStates();
	ServerWrapper GetCurrentGameType();
#pragma endregion
};