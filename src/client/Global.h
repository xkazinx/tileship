#pragma once

struct App;
struct SceneMgr;
struct Map;

#define DEBUG_MODE

namespace Global
{
	constexpr char kAppName[] = "Tileship";

	extern App * g_app;
	extern unique_ptr<Random> g_rand;
	extern unique_ptr<SceneMgr> g_scene;
	extern unique_ptr<SocketHandler> g_sock;
	extern Map * g_map;

	extern void Initialize();
	extern void Deinitialize();

	extern void Send(const OPktPtr & out, enet_uint8 channel = 0);

	namespace scene
	{
		enum ID : u8
		{
			Login,
			Main,
			Lobby,
			Game,
			Result,
		};
	}
}

namespace font
{
	enum class ID : u8
	{
		Arial,
		Max,
	};

	using FontNames = const vector<string>;
	extern FontNames kNames;
}

using SceneID = Global::scene::ID;
