#include "stdafx.h"
#include "App.h"
#include "Player.h"
#include "PlayerMgr.h"
#include <filesystem>

App::App()
	: Broadcaster<Op>(NetOp::Main)
{
	g_app = this;
	Global::Initialize();

	if (Sql::EnsureSchema())
		Info("Database ready");
	else
		Warn("The database is unreachable: logins and new accounts fail until it answers");

	const u16 port = ServerPort();
	g_sock = make_unique<SocketHandler>(Socket::Type::Host, ServerIp(), port, (enet_uint16)256, (enet_uint8)1);
	if (!g_sock->Get()->_host)
	{
		Err("Could not listen on port {}", port);
		std::exit(1);
	}
	Info("Listening on port {}", port);

	// Browser clients through the WebSocket gateway, as ENet peers on loopback
	if (const u16 ws_port = (u16)lib::init::Number("ws", port - 1); ws_port)
	{
		std::error_code ec;
		const auto web_dir = lib::exe::Dir() / "web";
		const string web = lib::init::Value("web", std::filesystem::is_directory(web_dir, ec) ? web_dir.string() : string());
		g_ws.Start({ .bind = lib::init::Value("ws_bind", "127.0.0.1"), .port = ws_port,
		             .enet_host = "127.0.0.1", .enet_port = port, .channels = 1, .max_clients = 256, .web_root = web });
	}
}

App::~App()
{
	Global::Deinitialize();
}

void App::NetOperationProcess()
{
	Info("NetOperationProcess: MainOp{}", Format(_op));

	switch (_op)
	{
		OP(Login);
		OP(CreateAccount);

	default:
		Err("Unknown MainOp{}", Format(_op));
		break;
	}
}

int App::Tick()
{
	while (true)
	{
		TickNetwork();

		this_thread::sleep_for(chrono::milliseconds(1));
	}

	return 0;
}

void App::TickNetwork()
{
	ENetEvent ev;
	for (auto e = g_sock->Service(&ev); e != Socket::Event::None; e = g_sock->Service(&ev))
	{
		switch (e)
		{
		case Socket::Event::Connect:	OnConnect(ev);		break;
		case Socket::Event::Packet:		OnPacket(ev);		break;
		case Socket::Event::Disconnect:	OnDisconnect(ev);	break;
		default:											break;
		}
	}
}

void App::OnConnect(ENetEvent & ev)
{
	Info("Client {} connected", Format(ev.peer));
	Socket::ApplyPeerTimeout(ev.peer);
}

void App::OnPacket(ENetEvent & ev)
{
	IPkt in(ev.packet); // destroys the enet packet when read

	auto * p = OnAuth(ev);
	if (!p)
	{
		Err("App::OnPacket: no free player slot for {}", Format(ev.peer));
		return;
	}

	using enum NetOp;
	switch (const auto op = in.ReadAs<NetOp>(); op)
	{
	case Main:	NetOperation(&in, p, ev.peer);	break;

	default: // #todo
		Err("Unknown NetOp{}", Format(op));
		break;
	}
}

void App::OnDisconnect(ENetEvent & ev)
{
	Info("Client {} disconnected", Format(ev.peer));
	g_player_mgr->Remove(ev.peer);
}

Player * App::OnAuth(const ENetEvent & ev) const
{
	if (Player * const p = g_player_mgr->Find(ev.peer))
		return p;

	for (PlayerIdx i = 1; i < player::kMax; i++)
	{
		auto & p = g_player_mgr->_array[i];
		if (!p)
		{
			p = make_unique<Player>(i, ev.peer);
			g_player_mgr->_list.push_back(p.get());
			Info("OnAuth: PlayerIdx {}", i);
			return p.get();
		}
	}

	return nullptr;
}

void App::NetOp_Login()
{
	const Auth auth(_in);
	if (_in->IsOverflow() || !auth.IsValid())
	{
		Err("NetOp_Login: Invalid Auth{}", Format(auth));
		Send(_player, MainOp::Login, false);
		return;
	}

	Info("NetOp_Login: Incoming Auth{}", Format(auth));

	const auto sql = Sql::Create();
	if (!sql)
	{
		Send(_player, MainOp::Login, false);
		return;
	}

	if (const auto res = sql->Select("players",
		{ "uid" },
		format(" WHERE `username` = {} AND `password` = SHA2(CONCAT(`salt`, {}), 256) LIMIT 1",
			sql->Quote(auth._user), sql->Quote(auth._pass))))
	{
		if (res->Pop())
		{
			Info("Login success for Auth{}", Format(auth));
			_player->_user = auth._user;
			Send(_player, MainOp::Login, true);
			return;
		}
	}

	Send(_player, MainOp::Login, false);
	Info("Login failure for Auth{}", Format(auth));
}

void App::NetOp_CreateAccount()
{
	const Auth auth(_in);
	if (_in->IsOverflow() || !auth.IsValid())
	{
		Err("NetOp_CreateAccount: Invalid Auth{}", Format(auth));
		Send(_player, MainOp::CreateAccount, false);
		return;
	}

	const auto sql = Sql::Create();
	if (!sql)
	{
		Send(_player, MainOp::CreateAccount, false);
		return;
	}

	const auto res = sql->Select("players",
		{ "uid" }, format(" WHERE `username` = {} LIMIT 1", sql->Quote(auth._user)));

	bool created = false;
	if (res && res->IsEmpty())
	{
		// A salt of its own: 32 hex digits
		static Random rand;
		static constexpr char kHex[] = "0123456789abcdef";
		string salt(32, '0');
		for (char & c : salt)
			c = kHex[rand.Generate(0, 15)];

		const string salt_q = sql->Quote(salt);
		created = sql->Query(format("INSERT INTO `players` (`username`, `salt`, `password`) VALUES ({}, {}, SHA2(CONCAT({}, {}), 256))",
			sql->Quote(auth._user), salt_q, salt_q, sql->Quote(auth._pass)));
	}

	Send(_player, MainOp::CreateAccount, created);

	if (created)
		Info("Create account success: username({})", auth._user);
	else
		Info("Create account failure: username({}) already in use", auth._user);
}
