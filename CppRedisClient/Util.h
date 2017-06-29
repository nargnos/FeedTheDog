#ifndef UTIL_H_
#define UTIL_H_

#include <initializer_list>
#include <string>
#include <ostream>
#include <vector>
#include <cassert>
#include <tuple>
constexpr const char* CRLF = "\r\n";

std::ostream& operator<<(std::ostream& s, const std::vector<char> & vec)
{
	std::ostreambuf_iterator<char> it(s);
	std::copy(vec.begin(), vec.end(), it);
	return s;
}
template<typename TIterator>
void _AppendValue(std::ostream & out,
	const std::tuple<TIterator, TIterator> & val)
{
	out << "$" << std::distance(std::get<0>(val), std::get<1>(val)) << CRLF;
	std::ostreambuf_iterator<char> it(out);
	std::copy(std::get<0>(val), std::get<1>(val), it);
	out << CRLF;
}
void _AppendValue(std::ostream & out, const std::vector<char> & val)
{
	out << "$" << val.size() << CRLF << val << CRLF;
}
void _AppendValue(std::ostream & out, const std::string& str)
{
	out << "$" << str.size() << CRLF << str << CRLF;
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


#endif // UTIL_H_

