
# ifndef SERVERCOMMANDS_H_
# define SERVERCOMMANDS_H_
# include "Util.h"
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

	class ClientKillSetter
	{
	public:

		// CLIENT KILL [ip:port] [ID client-id] [TYPE normal|master|slave|pubsub] [ADDR ip:port] [SKIPME yes/no]
		// Kill the connection of a client
		friend ClientKillSetter ClientKill(std::ostream& out)
		{
			return ClientKillSetter(out);
		}
		enum class Type :char
		{
			Normal,
			Master,
			Slave,
			Pubsub

		};
		~ClientKillSetter()
		{
			_AppendSize(out_, count_);
			_Append(out_, "CLIENT", "KILL");
			out_ << tmp_.str();
		}

		ClientKillSetter& SetAddr(const std::string& addr)
		{
			_Append(tmp_, "ADDR", addr);
			count_ += 2;
			return *this;
		}
		ClientKillSetter& SetClientId(const std::vector<char>& cid)
		{
			_Append(tmp_, "ID", cid);
			count_ += 2;
			return *this;
		}
		ClientKillSetter& SetType(ClientKillSetter::Type val)
		{
			switch (val)
			{
			case ClientKillSetter::Type::Normal:
				_Append(tmp_, "TYPE", "normal");
				break;
			case ClientKillSetter::Type::Master:
				_Append(tmp_, "TYPE", "master");
				break;
			case ClientKillSetter::Type::Slave:
				_Append(tmp_, "TYPE", "slave");
				break;
			case ClientKillSetter::Type::Pubsub:
				_Append(tmp_, "TYPE", "pubsub");
				break;
			default:
				assert(false);
				break;
			}
			count_ += 2;
			return *this;
		}
		ClientKillSetter& SetSkipMe(bool isSkipMe)
		{
			_Append(tmp_, "SKIPME", isSkipMe ? "yes" : "no");
			count_ += 2;
			return *this;
		}
	private:
		explicit ClientKillSetter(std::ostream& out) :
			out_(out),
			count_(2)
		{
		}
		std::ostringstream tmp_;
		std::ostream& out_;
		int count_;

	};



	// CLIENT LIST 
	// Get the list of client connections
	void ClientList(std::ostream& out)
	{
		GenCmd(out, "CLIENT", "LIST");
	}

	// CLIENT GETNAME 
	// Get the current connection name
	void ClientGetname(std::ostream& out)
	{
		GenCmd(out, "CLIENT", "GETNAME");
	}

	// CLIENT PAUSE timeout
	// Stop processing commands from clients for some time
	void ClientPause(std::ostream& out, int timeout)
	{
		GenCmd(out, "CLIENT", "PAUSE", timeout);
	}

	// CLIENT REPLY ON|OFF|SKIP
	// Instruct the server whether to reply to commands
	void ClientReply(std::ostream& out, ReplyModes onOffSkip)
	{
		GenCmd(out, "CLIENT", "REPLY", onOffSkip);
	}

	// CLIENT SETNAME connection-name
	// Set the current connection name
	void ClientSetname(std::ostream& out, const std::string& connectionName)
	{
		GenCmd(out, "CLIENT", "SETNAME", connectionName);
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
		GenCmd(out, "COMMAND", "COUNT");
	}

	// COMMAND GETKEYS 
	// Extract keys given a full Redis command
	void CommandGetkeys(std::ostream& out)
	{
		GenCmd(out, "COMMAND", "GETKEYS");
	}

	// COMMAND INFO command-name [command-name ...]
	// Get array of specific Redis command details
	void CommandInfo(std::ostream& out, const std::string& commandName, const std::vector<std::string>& commandNames = {})
	{
		GenCmdWithSize(out, 3 + commandNames.size(), "CLUSTER", "INFO", commandName, commandNames);
	}

	// CONFIG GET parameter
	// Get the value of a configuration parameter
	void ConfigGet(std::ostream& out, const std::string& parameter)
	{
		GenCmd(out, "CONFIG", "GET", parameter);
	}

	// CONFIG REWRITE 
	// Rewrite the configuration file with the in memory configuration
	void ConfigRewrite(std::ostream& out)
	{
		GenCmd(out, "CONFIG", "REWRITE");
	}

	// CONFIG SET parameter value
	// Set a configuration parameter to the given value
	void ConfigSet(std::ostream& out, const std::string& parameter, const std::string& value)
	{
		GenCmd(out, "CONFIG", "SET", parameter, value);
	}



	// CONFIG SET parameter value
	// Set a configuration parameter to the given value
	void ConfigSet(std::ostream& out, const std::string& parameter, const std::vector<char>& value)
	{
		GenCmd(out, "CONFIG", "SET", parameter, value);
	}

	// CONFIG RESETSTAT 
	// Reset the stats returned by INFO
	void ConfigResetstat(std::ostream& out)
	{
		GenCmd(out, "CONFIG", "RESETSTAT");
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
		GenCmd(out, "DEBUG", "OBJECT", key);
	}

	// DEBUG SEGFAULT 
	// Make the server crash
	void DebugSegfault(std::ostream& out)
	{
		GenCmd(out, "DEBUG", "SEGFAULT");
	}

	// FLUSHALL [ASYNC]
	// Remove all keys from all databases
	void Flushall(std::ostream& out, bool isAsync = false)
	{
		if (isAsync)
		{
			GenCmd(out, "FLUSHALL", "ASYNC");

		}
		else
		{
			GenCmd(out, "FLUSHALL");
		}
	}

	// FLUSHDB [ASYNC]
	// Remove all keys from the current database
	void Flushdb(std::ostream& out, bool isAsync = false)
	{
		if (isAsync)
		{
			GenCmd(out, "FLUSHDB", "ASYNC");

		}
		else
		{
			GenCmd(out, "FLUSHDB");
		}
	}

	// INFO [section]
	// Get information and statistics about the server
	void Info(std::ostream& out)
	{
		GenCmd(out, "INFO");
	}
	// INFO [section]
	// Get information and statistics about the server
	void Info(std::ostream& out, const std::string& section)
	{
		GenCmd(out, "INFO", section);
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
		GenCmd(out, "SHUTDOWN");
	}

	// SHUTDOWN [NOSAVE|SAVE]
	// Synchronously save the dataset to disk and then shut down the server
	void Shutdown(std::ostream& out, ShutdownModes mode)
	{
		GenCmd(out, "SHUTDOWN,mode");
	}
	// SLAVEOF host port
	// Make the server a slave of another instance, or promote it as master
	void Slaveof(std::ostream& out, const std::string& host, unsigned short port)
	{
		GenCmd(out, "SLAVEOF", host, port);
	}

	// SLOWLOG subcommand [argument]
	// Manages the Redis slow queries log
	void Slowlog(std::ostream& out, const std::string& subcommand)
	{
		GenCmd(out, "SLOWLOG", subcommand);
	}

	// SLOWLOG subcommand [argument]
	// Manages the Redis slow queries log
	void Slowlog(std::ostream& out, const std::string& subcommand, int argument)
	{
		GenCmd(out, "SLOWLOG", subcommand, argument);
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

# endif // SERVERCOMMANDS_H_
