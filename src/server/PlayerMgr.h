#pragma once

struct Player;

struct PlayerMgr
{
	array<unique_ptr<Player>, player::kMax> _array;
	vector<Player *> _list;

	PlayerMgr();
	~PlayerMgr();

	// The peer's player, or nullptr
	Player * Find(const ENetPeer * peer) const;
	// The peer's player gone (its slot free again)
	void Remove(const ENetPeer * peer);
};
