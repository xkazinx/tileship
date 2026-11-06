#pragma once
// Tileship's shared header (client and server; Windows, Linux, the browser):
// kazilib's std names, startup flags, log lines and enet packet layer, then the
// game's own types.
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
#include <cstddef>
#include <cstdint>
using i64 = std::int64_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using i8 = std::int8_t;
using u64 = std::uint64_t;
using u32 = std::uint32_t;
using u16 = std::uint16_t;
using u8 = std::uint8_t;
using szt = std::size_t;

// winsock2 before anything includes <windows.h> (initflags.h does), or windows.h
// brings the old winsock.h and enet's winsock2.h redefines it all
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#endif

#include <stdnames.h>
#include <initflags.h>
#include <under.h>
#include <log.h> // Out / Info / Warn / Err
#include <magic_enum.h>
#include <net/packet.h>
#include <net/socket.h>
#include <net/broadcaster.h>
#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <map>
#include <thread>

using lib::IPkt;
using lib::OPkt;
using lib::OPktPtr;
using lib::PktCap;
using lib::PktSz;
using lib::Under;
using Socket = lib::Socket;
using SocketHandler = lib::SocketHandler;
using magic_enum::enum_name;
using std::format;
using std::to_underlying;
using namespace std::string_literals;
namespace chrono = std::chrono;
namespace this_thread = std::this_thread;

namespace common
{
	// The server's address: launch flags ip=<host> port=<n>, or init.json beside the exe
	inline constexpr const char * kDefaultHost = "127.0.0.1";
	inline constexpr u16 kDefaultPort = 7021;
	inline string ServerIp() { return lib::init::Value("ip", kDefaultHost); }
	inline u16 ServerPort() { return (u16)lib::init::Number("port", kDefaultPort); }

	namespace tile
	{
		enum class Type : u8
		{
			Grass,
			Water,
			Rock,
			Max
		};

		using LineSize = u8;
		static constexpr float
			kSize = 32.f,
			kHalfSize = kSize / 2.f;

#ifdef CLIENT
		using ColorArray = const array<sf::Color, (szt)Type::Max>;
		extern ColorArray kColors;
#endif
	}

	namespace player
	{
		using Idx = u16;
		constexpr Idx
			kNullIdx = 0,
			kMax = 512;

		constexpr u8
			kNameSize = 32,
			kPasswordSize = 32;
	}

	using PlayerIdx = player::Idx;
}

#include "netops.h"
#include "broadcaster.h"
#include "random.h"
#include "auth.h"

namespace common
{
	enum class FormatType : u8
	{
		None,
		bool_Switch,	  // On, Off
		bool_State,		  // true, false
		bool_Status,	  // Enabled, Disabled
		bool_Result,	  // Success, Failure
		bool_Affirmation, // Yes, No
	};

	template <typename T>
	inline string Format(T v, FormatType type = FormatType::None)
	{
		if constexpr (std::is_enum_v<T>)
		{
			return format("({}:{})", to_underlying(v), enum_name(v));
		}
		else if constexpr (is_same_v<bool, T>)
		{
			using enum FormatType;
			switch (type)
			{
			case bool_Switch:		return v ? "On" : "Off";
			case bool_State:		return v ? "True" : "False";
			case bool_Status:		return v ? "Enabled" : "Disabled";
			case bool_Result:		return v ? "Success" : "Failure";
			case bool_Affirmation:	return v ? "Yes" : "No";
			default:				return format("{}", v);
			}
		}
		else if constexpr (is_same_v<ENetPeer *, T>)
		{
			return format("({}:{})", lib::EPacket::GetIP(v), v->address.port);
		}
		else if constexpr (is_same_v<Auth, T>)
		{
			return format("({})", v._user); // the password never printed
		}
		else
		{
			return to_string(v);
		}
	}
}

using namespace common;
