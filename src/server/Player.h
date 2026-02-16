#pragma once
#include "Unit.h"

class Player : public player::Base, public Unit, public Broadcaster<PlayerOp>
{
public:
	Player(PlayerIdx idx, ENetPeer* peer);
	~Player() override;
	
	void NetOperationProcess() override;
	
	PlayerIdx _idx;
	ENetPeer* _peer;
	string _ip;
	u16 _login_token;
};

