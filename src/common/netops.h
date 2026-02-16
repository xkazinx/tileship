#pragma once
#include <cassert>

namespace common
{
	enum class NetOp : u8
	{
		Main,
		Player,
		Disconnect,
		Map,
	};

	enum class MainOp : u8
	{
		Login,
		CreateAccount,
	};

	enum class PlayerOp : u8
	{
		Spawn,
		Max,
	};

	enum class MapOp : u8
	{
		UnitSpawn,
		SelfSpawn,
	};
}