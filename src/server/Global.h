#pragma once
#include <net/ws_gateway.h>

struct App;
struct PlayerMgr;
struct Player;

namespace Global
{
	constexpr char kAppName[] = "Tileship Server";

	extern App * g_app;
	extern unique_ptr<SocketHandler> g_sock;
	// Browser clients (the web build) in through WebSockets, as ENet peers of
	// g_sock (kazilib net/ws_gateway.h): flags ws=<port> (default the game port
	// - 1, 0 = off), ws_bind=<address> (default 127.0.0.1: a reverse proxy in
	// front), web=<folder> (the web build's files, served on the same port;
	// default the exe's web\ folder when there is one)
	extern lib::ws::Gateway g_ws;
	extern unique_ptr<PlayerMgr> g_player_mgr;

	extern void Initialize();
	extern void Deinitialize();

	extern void Send(const Player * player, const OPktPtr & out, enet_uint8 channel = 0);
}
