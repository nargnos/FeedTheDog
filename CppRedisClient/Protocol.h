#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
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

	template<typename T>
	static typename std::enable_if<!std::is_arithmetic<typename std::decay<T>::type>::value>::type
		_Append(std::ostream& output, T&& val)
	{
		std::string str = StrConv<typename std::decay<T>::type>::ToString(std::forward<T>(val));
		output << "$" << str.size() << CRLF;
		output << str << CRLF;
	}
	template<typename T>
	static typename std::enable_if<std::is_arithmetic<typename std::decay<T>::type>::value>::type
		_Append(std::ostream& output, T&& val)
	{
		std::string str = std::to_string(val);
		output << "$" << str.size() << CRLF;
		output << str << CRLF;
	}

	template<typename... TArgs>
	void BuildCommand(std::ostream& output, TArgs&&... args)
	{
		output << "*" << sizeof...(args) << CRLF;
		int ignore[]{ (_Append(output,std::forward<TArgs>(args)),0)... };
		(void)ignore;
	}


	enum class ResponseType :char
	{
		DataError, // 解析错误
		NotComplete, // 数据不完整
		Ok, // +
		Error, // -
		Int, // ：
		Bytes, // $
		Multi, // *
	};

	// 行类型 值左下标 值右下标（不包括crlf）
	using ResponseLine = std::tuple<ResponseType, size_t, size_t>;

	// 找CRLF，返回idx，找不到返回-1
	static int _FindEndIdx(const char* buff, size_t buffSize)
	{
		if (buffSize < 2)
		{
			return -1;
		}
		for (size_t i = 0; i < buffSize - 1; i++)
		{
			if (buff[i] == '\r' && buff[i + 1] == '\n')
			{
				return i;
			}
		}
		return -1;
	}
	// flag表示不出错时的行类型
	static ResponseLine _GetVal(const char* buff, size_t buffSize, ResponseType flag)
	{
		ResponseLine ret = std::make_tuple(ResponseType::DataError, 0, 0);
		if (flag != ResponseType::DataError)
		{
			auto findEnd = _FindEndIdx(buff + 1, buffSize - 1);

			if (findEnd == -1)
			{
				std::get<0>(ret) = ResponseType::NotComplete;
			}
			else
			{
				ret = std::make_tuple(flag, 1, findEnd + 1);
			}
		}
		return ret;
	}


	// 取得该行值的前后下标
	static ResponseLine GetLine(const char* buff, size_t buffSize)
	{
		ResponseType flag = ResponseType::DataError;
		switch (*buff)
		{
		case '+':
			flag = ResponseType::Ok;
			break;
		case '-':
			flag = ResponseType::Error;
			break;
		case ':':
			flag = ResponseType::Int;
			break;
		case '*':
			flag = ResponseType::Multi;
			break;
		case '$':
			flag = ResponseType::Bytes;
			break;
		default:
			break;
		}
		return _GetVal(buff, buffSize, flag);
	}



}
