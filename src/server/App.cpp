#include "stdafx.h"
#include "App.h"
#include "Player.h"
#include "PlayerMgr.h"

App::App()
	: Broadcaster<Op>(NetOp::Main)
{
	// #todo #param or whatever name it has for client and server
	g_app = this;
	Global::Initialize();
	using namespace std;
	g_sock = make_unique<Socket>(Socket::Type::Server, 
		bind(&App::OnConnect,	 this,	placeholders::_1), 
		bind(&App::OnPacket,	 this,	placeholders::_1),
		bind(&App::OnDisconnect, this,	placeholders::_1),
		256);
}

App::~App()
{
	Global::Deinitialize();
}

void App::NetOperationProcess()
{
	Print("NetOperationProcess: MainOp{}", Format(GetOperation()));

	switch(GetOperation())
	{
		OP(Login);

	default:
		Log::Error("Unknown MainOp{}", Format(GetOperation()));
		break;
	}
}

int App::Tick() 
{
	while (true)
	{
		g_sock->Tick();

		this_thread::sleep_for(chrono::milliseconds(1));
	}

	return 0;
}

void App::OnConnect([[maybe_unused]] ENetEvent& ev)
{
}

void App::OnPacket(ENetEvent& ev)
{
	auto* p = g_player_mgr->OnAuth(ev.peer);
	if (!p)
	{
		Log::Error("App::OnPacket::!p");
		return;
	}
	
	using enum NetOp;
	InPkt in(ev.packet);
	const auto op = in.ReadAs<NetOp>();

	const InOperation opt(in, p, ev.peer);

	switch (op)
	{
	case Main:	this->NetOperation(opt);	break;

	default: // #todo
		Log::Error("Unknown NetOp{}", Format(op));
		break;
	}
}

void App::OnDisconnect([[maybe_unused]] ENetEvent& ev)
{
	//g_player_mgr->_list.push_back(p.get());
	//g_player_mgr->_array[*player_idx].get();
}

void App::NetOp_Login()
{
	auto name = GetPacket()->ReadFixed(player::kNameSize + 1);

	for(auto & player : g_player_mgr->_list)
	{
		if(player->_name == name)
		{
			Send(GetPlayer(), MainOp::Login, false);
			return;
		}
	}

	GetPlayer()->_name = name;
	Send(GetPlayer(), MainOp::Login, true, GetPlayer()->_name);

	Log::Debug("Playyer logged in as {}", name);
}