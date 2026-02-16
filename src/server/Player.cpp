#include "stdafx.h"
#include "Player.h"

Player::Player(PlayerIdx idx, ENetPeer* peer) :				Broadcaster<PlayerOp>(NetOp::Player),
	_idx(idx), _peer(peer), _login_token(g_rnd->Generate<u16>(1u, std::numeric_limits<u16>::max()))
{

}

Player::~Player()
{

}

void Player::NetOperationProcess()
{
}
