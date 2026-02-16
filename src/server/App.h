#pragma once

class Player;

class App final : Broadcaster<MainOp>
{
public:
	using Op = MainOp;

	App();
	~App();

	int Tick();
	void OnConnect(ENetEvent & ev);
	void OnPacket(ENetEvent & ev);
	void OnDisconnect(ENetEvent & ev);

	void NetOperationProcess() override;

private:
	void NetOp_Login();
	void NetOp_CreateAccount();
};

