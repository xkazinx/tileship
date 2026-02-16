#include "stdafx.h"
#include "Global.h"
#include "Scenes.h"
#include "Units.h"
#include "Player.h"

namespace Global
{
	App* g_app;
	unique_ptr<Random> g_rand;
	unique_ptr<Scenes> g_scene;
	unique_ptr<Socket> g_sock;
	unique_ptr<Units> g_units;
	Player * g_player;
	Map* g_map;

	void Initialize()
	{
		// unordered
		g_rand = make_unique<Random>();
		g_scene = make_unique<Scenes>();
		g_units = make_unique<Units>();

		// ordered
	}

	void Deinitialize()
	{

	}

	void Send(const OutPktRef out, enet_uint8 channel)
{
		g_sock->Send(out, channel);
	}
}

namespace font
{
	FontNames kNames = 
	{
		"arial.ttf",
	};
}