#ifndef UTIL_H_
#define UTIL_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include <vector>
#include <cassert>
#include <tuple>
#include <sstream>
#include <chrono>
constexpr const char* CRLF = "\r\n";

#define CHECKITERATORTYPE(val) \
static_assert(std::is_same<char, std::decay<typename std::iterator_traits<decltype(std::begin((val)))>::value_type>::type>::value, "iterator type != char")

enum class InsertPos :char
{
	Before,
	After
};
enum class ReplyModes :char
{
	On,
	Off,
	Skip
};
enum class ScriptDebugModes :char
{
	Yes,
	Sync,
	No
};

enum class Operation :char
{
	And,
	Or,
	Xor,
	Not
};
enum class Distance :char
{
	M,
	KM,
	MI,
	FT
};
enum class SlotModes :char
{
	Importing,
	Migrating,
	Stable,
	Node
};

enum class FailOverModes:char
{
	Force,
	Takeover
};

enum class ResetModes:char
{
	Hard,
	Soft
};
enum class ShutdownModes:char
{
	Nosave,
	Save
};
std::ostream& operator<<(std::ostream& s, const std::vector<char> & vec)
{
	std::ostreambuf_iterator<char> it(s);
	std::copy(vec.begin(), vec.end(), it);
	return s;
}

void _AppendValue(std::ostream & out, const std::vector<char> & val)
{
	out << "$" << val.size() << CRLF << val << CRLF;
}
void _AppendValue(std::ostream & out, const std::string& str)
{
	out << "$" << str.size() << CRLF << str << CRLF;
}
using KeyValuePair = std::tuple<std::string, std::string>;
using KeyVectorPair = std::tuple<std::string, std::vector<char>>;
void _AppendValue(std::ostream & out, const KeyValuePair& val)
{
	_AppendValue(out, std::get<0>(val));
	_AppendValue(out, std::get<1>(val));
}
void _AppendValue(std::ostream & out, const KeyVectorPair& val)
{
	_AppendValue(out, std::get<0>(val));
	_AppendValue(out, std::get<1>(val));
}
template<typename T>
std::enable_if<!std::is_same<char, T>::value>
_AppendValue(std::ostream & out, const std::vector<T>& vals)
{
	if (vals.size() == 0)
	{
		return;
	}
	for (auto& i : vals)
	{
		_AppendValue(out, i);
	}
}
std::ostream& operator<<(std::ostream& s, InsertPos val)
{
	const std::string* str = nullptr;
	static std::string bf("BEFORE");
	static std::string af("AFTER");
	switch (val)
	{
	case InsertPos::Before:
		str = &bf;
		break;
	case InsertPos::After:
		str = &af;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}
std::ostream& operator<<(std::ostream& s, ReplyModes val)
{
	const std::string* str = nullptr;
	static std::string on("ON");
	static std::string off("OFF");
	static std::string skip("SKIP");
	switch (val)
	{
	case ReplyModes::On:
		str = &on;
		break;
	case ReplyModes::Off:
		str = &off;
		break;
	case ReplyModes::Skip:
		str = &skip;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}
std::ostream& operator<<(std::ostream& s, ScriptDebugModes val)
{
	const std::string* str = nullptr;
	static std::string yes("YES");
	static std::string no("NO");
	static std::string sync("SYNC");
	switch (val)
	{
	case ScriptDebugModes::Yes:
		str = &yes;
		break;
	case ScriptDebugModes::Sync:
		str = &sync;
		break;
	case ScriptDebugModes::No:
		str = &no;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}

std::ostream& operator<<(std::ostream& s, Operation val)
{
	const std::string* str = nullptr;
	static std::string andStr("AND");
	static std::string orStr("OR");
	static std::string notStr("NOT");
	static std::string xorStr("XOR");

	switch (val)
	{
	case Operation::And:
		str = &andStr;
		break;
	case Operation::Or:
		str = &orStr;
		break;
	case Operation::Xor:
		str = &notStr;
		break;
	case Operation::Not:
		str = &xorStr;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}

std::ostream& operator<<(std::ostream& s, Distance val)
{
	const std::string* str = nullptr;
	static std::string m("m");
	static std::string km("km");
	static std::string mi("mi");
	static std::string ft("ft");

	switch (val)
	{
	case Distance::M:
		str = &m;
		break;
	case Distance::KM:
		str = &km;
		break;
	case Distance::MI:
		str = &mi;
		break;
	case Distance::FT:
		str = &ft;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}
std::ostream& operator<<(std::ostream& s, SlotModes val)
{
	const std::string* str = nullptr;
	static std::string importing("IMPORTING");
	static std::string migrating("MIGRATING");
	static std::string stable("STABLE");
	static std::string node("NODE");

	switch (val)
	{
	case SlotModes::Importing:
		str = &importing;
		break;
	case SlotModes::Migrating:
		str = &migrating;
		break;
	case SlotModes::Stable:
		str = &stable;
		break;
	case SlotModes::Node:
		str = &node;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}
std::ostream& operator<<(std::ostream& s, FailOverModes val)
{
	const std::string* str = nullptr;
	static std::string force("FORCE");
	static std::string takeover("TAKEOVER");
	switch (val)
	{
	case FailOverModes::Force:
		str = &force;
		break;
	case FailOverModes::Takeover:
		str = &takeover;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}
std::ostream& operator<<(std::ostream& s, ResetModes val)
{
	const std::string* str = nullptr;
	static std::string hard("HARD");
	static std::string soft("SOFT");
	switch (val)
	{
	case ResetModes::Hard:
		str = &hard;
		break;
	case ResetModes::Soft:
		str = &soft;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}
std::ostream& operator<<(std::ostream& s, ShutdownModes val)
{
	const std::string* str = nullptr;
	static std::string nosave("NOSAVE");
	static std::string save("SAVE");
	switch (val)
	{
	case ShutdownModes::Nosave:
		str = &nosave;
		break;
	case ShutdownModes::Save:
		str = &save;
		break;
	default:
		assert(false);
		break;
	}
	_AppendValue(s, *str);
}

// TODO: 是否所有数字类型都可这么弄？
template<typename T>
std::enable_if<std::is_arithmetic<T>::value>
_AppendValue(std::ostream & out, T val)
{
	auto str = std::to_string(val);
	_AppendValue(out, str);
}
template<int Len>
void _AppendValue(std::ostream & out, const char(&str)[Len])
{
	std::ostreambuf_iterator<char> it(out);
	// 把尾0发过去无所谓，占一些空间而已
	if (str[Len - 1] == '\0')
	{
		out << "$" << Len - 1 << CRLF;
		std::copy(std::begin(str), std::end(str) - 1, it);
		out << CRLF;
		return;
	}
	out << "$" << Len << CRLF;
	std::copy(std::begin(str), std::end(str), it);
	out << CRLF;
}

void _AppendSize(std::ostream& out, size_t size)
{
	out << "*" << size << CRLF;
}
void _Append(std::ostream & out)
{}
template<typename TArg, typename... TArgs>
void _Append(std::ostream & out, TArg&& arg, TArgs&&... args)
{
	_AppendValue(out, std::forward<TArg>(arg));
	_Append(out, std::forward<TArgs>(args)...);
}

template<typename... TArgs>
void GenCmd(std::ostream & out, TArgs&&... args)
{
	_AppendSize(out, sizeof...(args));
	_Append(out, std::forward<TArgs>(args)...);
}
template<typename... TArgs>
void GenCmdWithSize(std::ostream & out, int size, TArgs&&... args)
{
	_AppendSize(out, size);
	_Append(out, std::forward<TArgs>(args)...);
}


#endif // UTIL_H_

