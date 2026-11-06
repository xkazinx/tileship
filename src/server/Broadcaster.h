#pragma once

struct Player;

// The server's broadcasters: a packet family's operations read from a player and
// sent to one (common/broadcaster.h)
template <class Op>
struct Broadcaster : broadcaster::Base<Op>
{
	explicit Broadcaster(const NetOp net_op) : broadcaster::Base<Op>(net_op) {}
	~Broadcaster() override = default;

	// <NetOp><op><args...> to the player
	template <typename... Args>
	void Send(const Player * const player, const Op op, const Args &... args) const
	{
		Global::Send(player, this->Pkt(op, args...));
	}

	// The packet's operation, then its handler (NetOperationProcess), with who sent it
	void NetOperation(IPkt * const in, Player * const player, ENetPeer * const peer)
	{
		_in = in;
		_player = player;
		_peer = peer;
		if (this->FetchOperation(in))
			NetOperationProcess();
	}

	virtual void NetOperationProcess() = 0;

	// The packet being handled (after its operation), its player and peer
	IPkt * _in = nullptr;
	Player * _player = nullptr;
	ENetPeer * _peer = nullptr;
};
