#include "stdafx.h"
#include "packet.h"

namespace common::packet
{
	Base::Base(ENetPacket* packet, const Size sz)
		: _ptr((char*)packet->data), _size(sz),
		_enet_packet(packet), _overflow(false)
	{

	}

	bool Base::ProcessOverflow(const bool extra_condition)
	{
		return (_overflow = extra_condition);
	}

	void Base::_Move(const Size pos, const bool is_out)
	{
		static_cast<Size&>(is_out ? GetLength() : _size) += pos;
		_ptr += pos;
	}

	Out::Out(const Size sz)
		: Base(enet_packet_create(nullptr, sz, ENET_PACKET_FLAG_RELIABLE), sz)
	{
		GetLength() = 0;
	}

	Out::~Out()
	{
		if (_enet_packet->referenceCount == 0)
		{
			enet_packet_destroy(_enet_packet);
		}
	}

	void Out::Send(ENetPeer* peer, const enet_uint8 channel) const
	{
		if (!peer || GetLength() == 0 || _overflow)
		{
			// #format add Format #todo option to do like... Format(const bool&, u8 format_type)... where you can do like True/False, or On/Off, etc.
			Log::Error("Can't send packet: Peer unavailable? ({}), EmptyPacket? (Length: {}), Overflow ({})", 
				Format(peer != nullptr, FormatType::bool_Affirmation), GetLength(),
				Format(_overflow, FormatType::bool_Affirmation));

			return;
		}

		enet_peer_send(peer, channel, _enet_packet);
	}

	void Out::Move(const Size sz)
	{
		_Move(sz, true);
	}

	bool Out::HasOverflowed(const Size  sz)
	{
		return ProcessOverflow(GetSize() - GetLength() < sz);
	}

	In::In(ENetPacket* p)
		: Base(p, 0)
	{

	}

	In::~In()
	{
		enet_packet_destroy(_enet_packet);
	}

	bool In::HasOverflowed(const Size sz)
	{
		return ProcessOverflow(GetLength() < GetSize() + sz);
	}

	void In::Move(const Size sz)
	{
		_Move(sz, false);
	}
}