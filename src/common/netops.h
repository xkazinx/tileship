#pragma once
#include <cassert>

namespace common
{
	enum class NetOp : u8
	{
		Main,
		Player,
		Disconnect,
	};

	enum class MainOp : u8
	{
		Login,
		CreateAccount,
	};

	enum class PlayerOp : u8
	{
		Max,
	};
}