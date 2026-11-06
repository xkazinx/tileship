#pragma once

struct Player
{
	Player(PlayerIdx idx, ENetPeer * peer);
	~Player();

	PlayerIdx _idx;
	ENetPeer * _peer;
	string _ip;
	// The account logged in on this connection ("" before)
	string _user;
};
