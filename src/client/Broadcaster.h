#pragma once

// The client's broadcasters: a packet family's operations sent to the server and
// read from it (common/broadcaster.h)
template <class Op>
struct Broadcaster : broadcaster::Base<Op>
{
	explicit Broadcaster(const NetOp net_op) : broadcaster::Base<Op>(net_op) {}
	~Broadcaster() override = default;

	// <NetOp><op><args...> to the server
	template <typename... Args>
	void Send(const Op op, const Args &... args) const
	{
		Global::Send(this->Pkt(op, args...));
	}

	// The packet's operation, then its handler (NetOperationProcess)
	void NetOperation(IPkt * const in)
	{
		_in = in;
		if (this->FetchOperation(in))
			NetOperationProcess();
	}

	virtual void NetOperationProcess() = 0;

	// The packet being handled, after its operation
	IPkt * _in = nullptr;
};
