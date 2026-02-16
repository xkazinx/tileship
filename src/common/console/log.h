#pragma once

enum class LogType
{
	Debug,
	Error,
	Out,
};

inline thread_local string kDummy;

template<LogType T>
class ConsoleOut
{
public:
	using Type = LogType;

	ConsoleOut(string & str) 
		: _str(str), _ptr(nullptr)
	{

	}

	ConsoleOut(const char * str)
		: _str(kDummy), _ptr(str)
	{

	}

	~ConsoleOut();

#ifndef CLIENT
	mutex _mutex;
#endif
	string & _str;
	const char * _ptr;
};

namespace Log
{
	using Type = LogType;	

	template <typename ... Args>
	void Debug(std::format_string<Args ...> fmt, Args && ... args) 
	{
		auto str = std::vformat(fmt.get(), std::make_format_args(args ...));
		ConsoleOut<Type::Debug>(str.c_str());
	}

	template <typename ... Args>
	void Error(std::format_string<Args ...> fmt, Args && ... args)
	{
		auto str = std::vformat(fmt.get(), std::make_format_args(args ...));
		ConsoleOut<Type::Error>(str.c_str());
	}
}

template <typename ... Args>
void Print(std::format_string<Args ...> fmt, Args && ... args)
{
	auto str = std::vformat(fmt.get(), std::make_format_args(args ...));
	ConsoleOut<LogType::Out>(str.c_str());
}
