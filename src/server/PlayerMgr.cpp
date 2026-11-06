#include "stdafx.h"
#include "PlayerMgr.h"
#include "Player.h"

PlayerMgr::PlayerMgr()
{
}

PlayerMgr::~PlayerMgr()
{
}

Player * PlayerMgr::Find(const ENetPeer * const peer) const
{
	for (Player * const p : _list)
		if (p->_peer == peer)
			return p;
	return nullptr;
}

void PlayerMgr::Remove(const ENetPeer * const peer)
{
	Player * const p = Find(peer);
	if (!p)
		return;

	std::erase(_list, p);
	_array[p->_idx].reset();
}
