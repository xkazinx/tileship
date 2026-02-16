#include "stdafx.h"
#include "Global.h"
#include "PlayerMgr.h"
#include "Player.h"
#include <random.h>
#include "GameMgr.h"
#include "Game.h"
#include "Map.h"

namespace Global
{
	App* g_app;
	unique_ptr<Socket> g_sock;
	unique_ptr<PlayerMgr> g_player_mgr;
	unique_ptr<GameMgr> g_games;

	void Initialize() {
		// unordered
		g_player_mgr = make_unique<PlayerMgr>();
		g_games = make_unique<GameMgr>();
		// ordered
	}

	void Deinitialize() {

	}

	void Send(const Player* player, OutPktRef& out, enet_uint8 channel)
	{
		g_sock->Send(player->_peer, out, channel);
	}
}