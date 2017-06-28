#ifndef UTIL_H_
#define UTIL_H_

#include <initializer_list>
#include <string>
#include <ostream>
#include <vector>
#include <cassert>

constexpr const char* CRLF = "\r\n";
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
void _AppendValue(std::ostream & out, int val)
{
	out << ":" << val << CRLF;
}
void _AppendValue(std::ostream & out, time_t t)
{
	out << ":" << t << CRLF;
}
void _AppendValue(std::ostream & out, const std::string& str)
{
	out << "$" << str.size() << CRLF << str << CRLF;
}
template<int Len>
void _AppendValue(std::ostream & out, const char(&str)[Len])
{
	std::ostreambuf_iterator<char> it(out);
	if (str[Len - 1] == '\0')
	{
		out << "$" << (Len - 1) << CRLF;
		std::copy(std::begin(str), std::end(str) - 1, it);
		out << CRLF;
	}
	else
	{
		out << "$" << Len << CRLF;
		std::copy(std::begin(str), std::end(str), it);
		out << CRLF;
	}
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

