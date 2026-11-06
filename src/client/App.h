#pragma once
#include "Broadcaster.h"

struct App final : Broadcaster<MainOp>
{
	using Op = MainOp;

	App();
	~App();

	void NetOperationProcess() override;

	int Tick();

	// false if the processed event has closed the window
	bool HandleEvents();

	RenderWindow _window;

	// The server: connected, and when the last attempt started (a lost or refused
	// connection is tried again every kRetryMs)
	bool _connected = false;
	bool _connecting = false;
	chrono::steady_clock::time_point _connect_at{};
	static constexpr int kRetryMs = 3000;

	// The account logged in ("" before)
	string _user;

private:
	void InitializeWindow();
	void Connect();
	void TickNetwork();
	void Frame(sf::Clock & delta_clock);
	void Shot();

	// shot=<file.png> (tests): the frames drawn in the game scene, and done
	static constexpr int kShotFrames = 30;
	int _game_frames = 0;
	bool _shot_done = false;

	void OnConnect(ENetEvent & ev);
	void OnPacket(ENetEvent & ev);
	void OnDisconnect(ENetEvent & ev);

	void NetOp_Login();
	void NetOp_CreateAccount();
};
