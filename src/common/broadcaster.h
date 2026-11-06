#pragma once
// The game's broadcasters, on kazilib's (net/broadcaster.h): a family of packets
// under one NetOp, each told apart by its own operation, <NetOp><Op><fields...>,
// every packet of a static tier summed from its fields' types (OPkt::Create).
// The client's and the server's own Broadcaster (src/client, src/server) add
// how a packet is sent and who it came from.

namespace common::broadcaster
{
	template <class Op>
	using Base = lib::BroadcasterBase<NetOp, Op, lib::PktSizing::Static>;
}

#define OP(op) case Op::op: NetOp_##op(); break;
