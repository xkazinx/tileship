#include "stdafx.h"
#include "Player.h"

Player::Player(const PlayerIdx idx, ENetPeer * const peer) :
	_idx(idx), _peer(peer), _ip(lib::EPacket::GetIP(peer))
{
}

Player::~Player()
{
}
