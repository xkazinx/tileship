#pragma once

class Player : public Broadcaster<PlayerOp>
{
public:
	Player(PlayerIdx idx, ENetPeer* peer);
	~Player();
	
	void NetOperationProcess() override;
	
	PlayerIdx _idx;
	ENetPeer* _peer;
	string _ip;
	u16 _login_token;
	string _name;
};

