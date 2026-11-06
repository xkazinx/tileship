#include "stdafx.h"
#include "App.h"
#include "SceneMgr.h"
#include "LoginScene.h"

App::App()
	: Broadcaster<Op>(NetOp::Main)
{
	g_app = this;
	Global::Initialize();

	InitializeWindow();

	ImGui::SFML::Init(_window);
	ImGui::GetIO().IniFilename = nullptr; // the windows have fixed places: no imgui.ini

#ifdef __EMSCRIPTEN__
	// A browser reaches the server through its WebSocket gateway (kazilib
	// net/ws_gateway.h, enet/websocket.cpp): ?ws=wss://host/path/ws, else the
	// page's own folder + "ws"
	if (const string ws = lib::init::Value("ws"); !ws.empty())
		enet_ws_set_url(ws.c_str());
#endif
	g_sock = make_unique<SocketHandler>(Socket::Type::Client, ServerIp(), ServerPort(), 1, 1);
	Info("Server: {}:{}", ServerIp(), ServerPort());
	Connect();

	g_scene->Set(SceneID::Login);
}

App::~App()
{
	if (g_sock)
		g_sock->Disconnect();
	ImGui::SFML::Shutdown();
	Global::Deinitialize();
	g_sock.reset();
}

void App::NetOperationProcess()
{
	Out("NetOperationProcess MainOp{}", Format(_op));

	switch (_op)
	{
		OP(Login);
		OP(CreateAccount);

	default:
		Err("Unknown MainOp{}", Format(_op));
		break;
	}
}

void App::InitializeWindow()
{
	ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antiAliasingLevel = 0;
#ifndef __EMSCRIPTEN__
	settings.majorVersion = 3;
	settings.minorVersion = 2;
#endif

	_window.create(VideoMode({ g_res_x, g_res_y }), kAppName, sf::Style::Default, sf::State::Windowed, settings);
	_window.setVerticalSyncEnabled(true);
}

void App::Connect()
{
	_connected = false;
	_connecting = true;
	_connect_at = chrono::steady_clock::now();
	g_sock->Connect();
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

	// Not connected (refused, lost, the server restarting): try again, kRetryMs
	// after the last attempt ended, or once one has gone unanswered past enet's timeout
	const auto wait = chrono::milliseconds(_connecting ? Socket::kPeerTimeoutMs + 2000 : kRetryMs);
	if (!_connected && chrono::steady_clock::now() - _connect_at > wait)
		Connect();
}

int App::Tick()
{
	sf::Clock delta_clock;

	while (_window.isOpen())
	{
		// Events
		if (!HandleEvents())
			break;

		// Network
		TickNetwork();

		Frame(delta_clock);

#ifndef __EMSCRIPTEN__
		// A browser waits in display() for its next animation frame; a native
		// window has vsync and this
		this_thread::sleep_for(chrono::milliseconds(1));
#endif
	}

	return 0;
}

void App::Frame(sf::Clock & delta_clock)
{
	// Update
	g_scene->Tick();
	ImGui::SFML::Update(_window, delta_clock.restart());

	// Render
	_window.clear(sf::Color::Black);
	g_scene->Render();
	ImGui::SFML::Render(_window);
	Shot();
	_window.display();
}

void App::Shot()
{
	// Tests: flag shot=<file.png> saves the window kShotFrames frames into the game
	// (quitaftershot: then closes)
	static const string shot = lib::init::Value("shot");
	if (shot.empty() || _shot_done || g_scene->_id != SceneID::Game || ++_game_frames < kShotFrames)
		return;

	_shot_done = true;
	sf::Texture tex(_window.getSize());
	tex.update(_window);
	if (tex.copyToImage().saveToFile(shot))
		Info("Screenshot: {}", shot);
	else
		Err("Screenshot failed: {}", shot);
	if (lib::init::Flag("quitaftershot", false))
		_window.close();
}

bool App::HandleEvents()
{
	while (const std::optional<sf::Event> ev = _window.pollEvent())
	{
		ImGui::SFML::ProcessEvent(*ev);

		if (ev->is<sf::Event::Closed>())
		{
			_window.close();
			return false;
		}

		g_scene->HandleEvent(*ev);
	}

	return true;
}

void App::OnConnect([[maybe_unused]] ENetEvent & ev)
{
	Info("Connected to the server");
	_connected = true;
	_connecting = false;
	auto & login = *g_scene->_login;
	login._status = "Connected";

	// Tests and demos: flags user=<name> pass=<password> log in at once (create:
	// the account made first)
	const string user = lib::init::Value("user"), pass = lib::init::Value("pass");
	if (!user.empty() && !pass.empty() && _user.empty())
	{
		std::snprintf(login._user, sizeof(login._user), "%s", user.c_str());
		std::snprintf(login._pass, sizeof(login._pass), "%s", pass.c_str());
		const Auth auth(login._user, login._pass);
		if (lib::init::Flag("create", false))
			Send(MainOp::CreateAccount, auth);
		login._sent_user = auth._user;
		Send(MainOp::Login, auth);
	}
}

void App::OnDisconnect([[maybe_unused]] ENetEvent & ev)
{
	Info("Disconnected from the server");
	const bool was = _connected;
	_connected = false;
	_connecting = false;
	_connect_at = chrono::steady_clock::now();
	_user.clear();
	g_scene->_login->_status = was ? "Disconnected from the server" : "Server unreachable, retrying";
	g_scene->Set(SceneID::Login);
}

void App::OnPacket(ENetEvent & ev)
{
	IPkt in(ev.packet); // destroys the enet packet when read
	using enum NetOp;
	switch (const auto op = in.ReadAs<NetOp>(); op)
	{
	case Main:	NetOperation(&in);	break;
	default:
		Err("Unknown NetOp{}", Format(op));
		break;
	}
}

void App::NetOp_Login()
{
	const bool ok = _in->ReadAs<bool>();
	auto & login = *g_scene->_login;
	if (!ok)
	{
		login._status = "Login failed: wrong username or password";
		Info("Login refused for {}", login._sent_user);
		return;
	}

	_user = login._sent_user;
	Info("Logged in as {}", _user);
	login._status.clear();
	g_scene->Set(SceneID::Game);
}

void App::NetOp_CreateAccount()
{
	const bool ok = _in->ReadAs<bool>();
	g_scene->_login->_status = ok ? "Account created: you can log in" : "That username is taken";
	Info("{}", g_scene->_login->_status);
}
