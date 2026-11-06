#include "stdafx.h"
#include "Global.h"
#include "SceneMgr.h"

namespace Global
{
	App * g_app;
	unique_ptr<Random> g_rand;
	unique_ptr<SceneMgr> g_scene;
	unique_ptr<SocketHandler> g_sock;
	Map * g_map;

	void Initialize()
	{
		// unordered
		g_rand = make_unique<Random>();
		g_scene = make_unique<SceneMgr>();

		// ordered
	}

	void Deinitialize()
	{
		g_scene.reset();
	}

	void Send(const OPktPtr & out, const enet_uint8 channel)
	{
		if (g_sock)
			out->Send(g_sock->Get(), channel);
	}
}

namespace font
{
	FontNames kNames =
	{
		"arial.ttf",
	};
}
