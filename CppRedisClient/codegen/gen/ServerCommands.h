
#ifndef SERVERCOMMANDS_H_
#define SERVERCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace ServerCommands
{

	// BGREWRITEAOF 
	// Asynchronously rewrite the append-only file
	void Bgrewriteaof(std::ostream& out)
	{
		GenCmd(out, "BGREWRITEAOF");
	}

	// BGSAVE 
	// Asynchronously save the dataset to disk
	void Bgsave(std::ostream& out)
	{
		GenCmd(out, "BGSAVE");
	}

	// CLIENT KILL [ip:port] [ID client-id] [TYPE normal|master|slave|pubsub] [ADDR ip:port] [SKIPME yes/no]
	// Kill the connection of a client
	void ClientKill(std::ostream& out)
	{
		// TODO: CLIENT KILL
		assert(false);
	}

	// CLIENT LIST 
	// Get the list of client connections
	void ClientList(std::ostream& out)
	{
		GenCmd(out, "CLIENT LIST");
	}

	// CLIENT GETNAME 
	// Get the current connection name
	void ClientGetname(std::ostream& out)
	{
		GenCmd(out, "CLIENT GETNAME");
	}

	// CLIENT PAUSE timeout
	// Stop processing commands from clients for some time
	void ClientPause(std::ostream& out, int timeout)
	{
		GenCmd(out, "CLIENT PAUSE", timeout);
	}

	// CLIENT REPLY ON|OFF|SKIP
	// Instruct the server whether to reply to commands
	void ClientReply(std::ostream& out)
	{
		// TODO: CLIENT REPLY
		assert(false);
	}

	// CLIENT SETNAME connection-name
	// Set the current connection name
	void ClientSetname(std::ostream& out)
	{
		// TODO: CLIENT SETNAME
		assert(false);
	}

	// COMMAND 
	// Get array of Redis command details
	void Command(std::ostream& out)
	{
		GenCmd(out, "COMMAND");
	}

	// COMMAND COUNT 
	// Get total number of Redis commands
	void CommandCount(std::ostream& out)
	{
		GenCmd(out, "COMMAND COUNT");
	}

	// COMMAND GETKEYS 
	// Extract keys given a full Redis command
	void CommandGetkeys(std::ostream& out)
	{
		GenCmd(out, "COMMAND GETKEYS");
	}

	// COMMAND INFO command-name [command-name ...]
	// Get array of specific Redis command details
	void CommandInfo(std::ostream& out)
	{
		// TODO: COMMAND INFO
		assert(false);
	}

	// CONFIG GET parameter
	// Get the value of a configuration parameter
	void ConfigGet(std::ostream& out)
	{
		// TODO: CONFIG GET
		assert(false);
	}

	// CONFIG REWRITE 
	// Rewrite the configuration file with the in memory configuration
	void ConfigRewrite(std::ostream& out)
	{
		GenCmd(out, "CONFIG REWRITE");
	}

	// CONFIG SET parameter value
	// Set a configuration parameter to the given value
	void ConfigSet(std::ostream& out)
	{
		// TODO: CONFIG SET
		assert(false);
	}

	// CONFIG RESETSTAT 
	// Reset the stats returned by INFO
	void ConfigResetstat(std::ostream& out)
	{
		GenCmd(out, "CONFIG RESETSTAT");
	}

	// DBSIZE 
	// Return the number of keys in the selected database
	void Dbsize(std::ostream& out)
	{
		GenCmd(out, "DBSIZE");
	}

	// DEBUG OBJECT key
	// Get debugging information about a key
	void DebugObject(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "DEBUG OBJECT", key);
	}

	// DEBUG SEGFAULT 
	// Make the server crash
	void DebugSegfault(std::ostream& out)
	{
		GenCmd(out, "DEBUG SEGFAULT");
	}

	// FLUSHALL [ASYNC]
	// Remove all keys from all databases
	void Flushall(std::ostream& out)
	{
		// TODO: FLUSHALL
		assert(false);
	}

	// FLUSHDB [ASYNC]
	// Remove all keys from the current database
	void Flushdb(std::ostream& out)
	{
		// TODO: FLUSHDB
		assert(false);
	}

	// INFO [section]
	// Get information and statistics about the server
	void Info(std::ostream& out)
	{
		// TODO: INFO
		assert(false);
	}

	// LASTSAVE 
	// Get the UNIX time stamp of the last successful save to disk
	void Lastsave(std::ostream& out)
	{
		GenCmd(out, "LASTSAVE");
	}

	// MONITOR 
	// Listen for all requests received by the server in real time
	void Monitor(std::ostream& out)
	{
		GenCmd(out, "MONITOR");
	}

	// ROLE 
	// Return the role of the instance in the context of replication
	void Role(std::ostream& out)
	{
		GenCmd(out, "ROLE");
	}

	// SAVE 
	// Synchronously save the dataset to disk
	void Save(std::ostream& out)
	{
		GenCmd(out, "SAVE");
	}

	// SHUTDOWN [NOSAVE|SAVE]
	// Synchronously save the dataset to disk and then shut down the server
	void Shutdown(std::ostream& out)
	{
		// TODO: SHUTDOWN
		assert(false);
	}

	// SLAVEOF host port
	// Make the server a slave of another instance, or promote it as master
	void Slaveof(std::ostream& out)
	{
		// TODO: SLAVEOF
		assert(false);
	}

	// SLOWLOG subcommand [argument]
	// Manages the Redis slow queries log
	void Slowlog(std::ostream& out)
	{
		// TODO: SLOWLOG
		assert(false);
	}

	// SYNC 
	// Internal command used for replication
	void Sync(std::ostream& out)
	{
		GenCmd(out, "SYNC");
	}

	// TIME 
	// Return the current server time
	void Time(std::ostream& out)
	{
		GenCmd(out, "TIME");
	}

} // namespace ServerCommands

#endif // SERVERCOMMANDS_H_
