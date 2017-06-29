
#ifndef CONNECTIONCOMMANDS_H_
#define CONNECTIONCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace ConnectionCommands
{

	// AUTH password
	// Authenticate to the server
	void Auth(std::ostream& out, const std::string& passwd)
	{
		GenCmd(out, "AUTH", passwd);
	}

	// ECHO message
	// Echo the given string
	void Echo(std::ostream& out, const std::string& msg)
	{
		GenCmd(out, "ECHO", msg);
	}

	// PING [message]
	// Ping the server
	void Ping(std::ostream& out)
	{
		// TODO: PING
		assert(false);
	}

	// QUIT 
	// Close the connection
	void Quit(std::ostream& out)
	{
		GenCmd(out, "QUIT");
	}

	// SELECT index
	// Change the selected database for the current connection
	void Select(std::ostream& out, int index)
	{
		GenCmd(out, "SELECT", index);
	}

	// SWAPDB index index
	// Swaps two Redis databases
	void Swapdb(std::ostream& out, int index1, int index2)
	{
		GenCmd(out, "SWAPDB", index1, index2);
	}

} // namespace ConnectionCommands

#endif // CONNECTIONCOMMANDS_H_
