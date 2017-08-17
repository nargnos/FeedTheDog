#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <cassert>
namespace Protocol
{
	constexpr const char* CRLF = "\r\n";
	// 其它不支持转换的格式需要新建其它具体化类型
	template<typename T>
	struct StrConv
	{
		static std::string ToString(const T& val)
		{
			return std::string(val);
		}
	};

	using Command = std::vector<std::string>;

	template<>
	struct StrConv<std::string>
	{
		static const std::string& ToString(const std::string& val)
		{
			return val;
		}
	};
	template<>
	struct StrConv<std::vector<char>>
	{
		static std::string ToString(const std::vector<char>& val)
		{
			return std::string(val.begin(), val.end());
		}
	};
	// 用来占位
	struct EmptyType
	{
		operator std::string()const
		{
			return{};
		}
	};
	constexpr EmptyType Empty{};

	template<>
	struct StrConv<EmptyType>
	{
		static std::string ToString(const EmptyType& val)
		{
			return{};
		}
	};

	template<>
	struct StrConv<Command>
	{
		static void ToString(const Command& cmd, std::ostream& output)
		{
			output << "*" << cmd.size() << CRLF;
			for (auto& i : cmd)
			{
				output << "$" << i.size() << CRLF;
				output << i << CRLF;
			}
		}
	};

	std::ostream& operator<<(std::ostream& s, const Command& cmd)
	{
		StrConv<Command>::ToString(cmd, s);
		return s;
	}

	template<typename T>
	typename std::enable_if<!std::is_arithmetic<typename std::decay<T>::type>::value, Command&>::type
		operator<<(Command& cmd, T&& val)
	{
		cmd.emplace_back(StrConv<typename std::decay<T>::type>::ToString(std::forward<T>(val)));
		return cmd;
	}
	template<typename T>
	typename std::enable_if<std::is_arithmetic<typename std::decay<T>::type>::value, Command&>::type
		operator<<(Command& cmd, T&& val)
	{
		cmd.emplace_back(std::to_string(val));
		return cmd;
	}


	class Response
	{
	public:
		Response()
		{
		}

		~Response()
		{
		}

	private:

	};

}
