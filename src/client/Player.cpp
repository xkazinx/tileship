#include "stdafx.h"
#include "Player.h"
#include "Map.h"
#include "GameScene.h"

string Player::LoginName;

Player::Player()
	: Broadcaster<PlayerOp>(NetOp::Player)
{

}

Player::~Player()
{
}

void Player::NetOperationProcess()
{
	switch(GetOperation())
	{
		OP(Spawn);
	}
}

void Player::NetOp_Spawn()
{
	for(player::GameIdx i = 0; i < _g->_players.size(); ++i)
	{
		auto & p = _g->_players[i];
		auto id = GetPacket()->ReadAs<PlayerID>();
		if(id == player::kNullID)
			continue;

		p = make_unique<Player>();
		p->_game_idx = i;
		p->_id = id;
		GetPacket()->Read(p->_pt, p->_name);

		if(p->_name == LoginName)
		{
			_p = p.get();
		}
	}

	Print("Received player spawn data");
}