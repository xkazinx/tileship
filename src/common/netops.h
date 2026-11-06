#pragma once

namespace common
{
	// Every packet starts with one (u8 on the wire, values only appended)
	enum class NetOp : u8
	{
		Main,
		Disconnect,
	};

	// NetOp::Main's operations (u8, values only appended)
	enum class MainOp : u8
	{
		Login,			// c->s Auth; s->c bool accepted
		CreateAccount,	// c->s Auth; s->c bool created
	};
}
