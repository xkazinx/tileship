#pragma once
#include "Unit.h"

class Player : public player::Base, public Unit, public Broadcaster<PlayerOp>
{
public:
	using Op = PlayerOp;

	Player();
	~Player() override;
	
	static string LoginName;

	void NetOperationProcess() override;
	void NetOp_Spawn();
};

