#pragma once

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <string>
using std::string;
using std::to_string;
using namespace std::string_literals;

#include <type_traits>
using i64 = std::int64_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using i8 = std::int8_t;

using u64 = std::uint64_t;
using u32 = std::uint32_t;
using u16 = std::uint16_t;
using u8 = std::uint8_t;

using std::size_t;
using szt = size_t;

#include <memory>
using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;

#include <vector>
using std::vector;

#include <array>
using std::array;

#include <map>

#include <thread>
namespace this_thread = std::this_thread;

#include <mutex>
using std::mutex;
using unique_mutex = std::unique_lock<mutex>;

#include <chrono>
namespace chrono = std::chrono;

#include <format>
using std::format;

#include <functional>
using std::function;

#include <cassert>

using std::copy;
using std::copy_n;
using std::move;
using std::forward;
using std::to_underlying;

using std::enable_if_t;
using std::is_integral_v;
using std::is_enum_v;
using std::is_unsigned_v;
using std::is_signed_v;
using std::is_same_v;
using std::underlying_type_t;
using std::is_base_of_v;
using std::is_pointer_v;

template <typename T, typename... Ts>
concept is_any = std::disjunction_v<std::is_same<T, Ts>...>;

/*
template <class T>
struct is_string : std::bool_constant<
	is_same_v<string, T> ||
	is_same_v<const string, T> ||
	is_same_v<string&, T> ||
	is_same_v<const string&, T>>{};
	*/

template <typename T>
concept is_string = is_any<T, string, const string, string&, const string &>;

#include <magic_enum.h>
using namespace magic_enum;

#include <console/log.h>

#define DEBUG_PACKET

#include <netops.h>
#include <random.h>
#include <packet.h>
#include <socket.h>
#include <broadcaster.h>

namespace common
{
	extern unique_ptr<Random> g_rnd;

constexpr u16 kPort = 321;
constexpr const char * kHost = "localhost";
/*
template <typename T>
constexpr underlying_type_t<T> to_underlying(T value) noexcept
{
	return underlying_type_t<T>(value);
}*/

template<class T>
class Point
{
public:
	constexpr Point() {}
	constexpr Point(const T & x_, const T & y_) : x(x_), y(y_) {}
	T x, y;
};

using RVal = i16;
using TPt = Point<RVal>;
using RPt = Point<RVal>;
using MPt = Point<float>;

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
	static constexpr u8
		kSize = 32,
		kHalfSize = kSize / 2;

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

	using GameIdx = u8;
	constexpr GameIdx kMaxGame = 4;

	using ID = u32;
	constexpr ID kNullID = 0;

	class Base
	{
	public:
		Base(){}
		virtual ~Base(){}

		ID _id;
		string _name;
		player::GameIdx _game_idx;
	};
}

using PlayerIdx = player::Idx;
using PlayerID = player::ID;


enum class FormatType : u8
{
	None,
	bool_Switch,	  // On, Off
	bool_State,		  // true, false
	bool_Status,	  // Enabled, Disabled
	bool_Result,	  // Success, Failure
	bool_Affirmation, // Yes, No
};

template<typename T>
inline string Format(T v, FormatType type = FormatType::None)
{
	if constexpr (is_enum_v<T>)
	{
		return format("({}:{})", to_underlying(v), enum_name(v));
	}
	else if constexpr (is_same_v<bool, T>)
	{
		using enum FormatType;
		switch (type)
		{
		case bool_Switch:		return format("{}", v ? "On" : "Off");
		case bool_State:		return format("{}", v ? "True" : "False");
		case bool_Status:		return format("{}", v ? "Enabled" : "Disabled");
		case bool_Result:		return format("{}", v ? "Success" : "Failure");
		case bool_Affirmation:	return format("{}", v ? "Yes" : "No");
		default:				return format("{}", v);
		}
	}
	else if constexpr (is_same_v<ENetPeer*, T>)
	{
		return format("({}:{}:PlayerIdx({}))", Socket::FetchIP(v), v->address.port, 
			v->data ? *reinterpret_cast<PlayerIdx*>(v->data) : player::kNullIdx);
	}
	/*else if constexpr (is_same_v<NetString, T>)
	{
		return format("({}:{})", v.IsChar() ? v._data : v._str);
	}*/
	else
	{
		return to_string(v);
	}
}

namespace unit
{
	enum class Action : u8
	{
		Move,
		Stop,
		Max,
	};
}

enum class Direction : u8
{
	North,
	East,
	South,
	West,
	Max
};

namespace dir
{
	inline Point<i8> GetDelta(Direction dir)
	{
		static array<Point<i8>, to_underlying(Direction::Max)> deltas
		{
			Point<i8>{ 0, 1 },
			{ 1, 0 },
			{ 0, -1 },
			{ -1, 0 }
		};

		if(to_underlying(dir) >= deltas.size())
			return { 0, 0 };

		return deltas[to_underlying(dir)];
	}

	inline Direction GetContrary(Direction dir)
	{
		using enum Direction;
		switch (dir) 
		{
		case North:		return South;
		case East:		return West;
		case South:		return North;
		case West:		return East;
		}

		return Max;
	}
}

constexpr auto kTileSz = tile::kSize;

namespace map
{
	class Base
	{
	public:
		Base(const TPt & size) : _size(size) {
		}

		virtual ~Base() {}

		TPt _size;
	};
}

namespace game
{
	using ID = u32;

}
}

using namespace common;