#include "stdafx.h"
#include "Global.h"
#include "PlayerMgr.h"
#include "Player.h"

namespace Global
{
	App * g_app;
	unique_ptr<SocketHandler> g_sock;
	lib::ws::Gateway g_ws;
	unique_ptr<PlayerMgr> g_player_mgr;

	void Initialize()
	{
		// unordered
		g_player_mgr = make_unique<PlayerMgr>();

		// ordered
	}

	void Deinitialize()
	{
		g_ws.Stop();
		g_player_mgr.reset();
	}

	void Send(const Player * const player, const OPktPtr & out, const enet_uint8 channel)
	{
		if (player)
			out->Send(player->_peer, channel);
	}
}
