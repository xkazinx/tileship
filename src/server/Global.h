#pragma once

class App;
class PlayerMgr;
class Player;
class Random;
class GameMgr;

namespace Global
{
	constexpr char kAppName[] = "Tileship Server";

	extern App* g_app;
	extern unique_ptr<Socket> g_sock;
	extern unique_ptr<PlayerMgr> g_player_mgr;
	extern unique_ptr<GameMgr> g_games;

	extern void Initialize();
	extern void Deinitialize();

	extern void Send(const Player* player, OutPktRef& out, enet_uint8 channel = 0);
}