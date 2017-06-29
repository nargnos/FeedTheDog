
#ifndef GENERICCOMMANDS_H_
#define GENERICCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace GenericCommands
{

	// DEL key [key ...]
	// Delete a key
	void Del(std::ostream& out)
	{
		// TODO: DEL
		assert(false);
	}

	// DUMP key
	// Return a serialized version of the value stored at the specified key.
	void Dump(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "DUMP", key);
	}

	// EXISTS key [key ...]
	// Determine if a key exists
	void Exists(std::ostream& out)
	{
		// TODO: EXISTS
		assert(false);
	}

	// EXPIRE key seconds
	// Set a key's time to live in seconds
	void Expire(std::ostream& out, const std::string& key, int sec)
	{
		GenCmd(out, "EXPIRE", key, sec);
	}

	// EXPIREAT key timestamp
	// Set the expiration for a key as a UNIX timestamp
	void Expireat(std::ostream& out, const std::string& key, time_t t)
	{
		GenCmd(out, "EXPIREAT", key, t);
	}

	// KEYS pattern
	// Find all keys matching the given pattern
	void Keys(std::ostream& out, const std::string& pattern)
	{
		GenCmd(out, "KEYS", pattern);
	}

	// MIGRATE host port key|"" destination-db timeout [COPY] [REPLACE] [KEYS key [key ...]]
	// Atomically transfer a key from a Redis instance to another one.
	void Migrate(std::ostream& out)
	{
		// TODO: MIGRATE
		assert(false);
	}

	// MOVE key db
	// Move a key to another database
	void Move(std::ostream& out, const std::string& key, int db)
	{
		GenCmd(out, "MOVE", key, db);
	}

	// OBJECT subcommand [arguments [arguments ...]]
	// Inspect the internals of Redis objects
	void Object(std::ostream& out)
	{
		// TODO: OBJECT
		assert(false);
	}

	// PERSIST key
	// Remove the expiration from a key
	void Persist(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "PERSIST", key);
	}

	// PEXPIRE key milliseconds
	// Set a key's time to live in milliseconds
	void Pexpire(std::ostream& out, const std::string& key, int ms)
	{
		GenCmd(out, "PEXPIRE", key, ms);
	}

	// PEXPIREAT key milliseconds-timestamp
	// Set the expiration for a key as a UNIX timestamp specified in milliseconds
	void Pexpireat(std::ostream& out, const std::string& key, time_t t)
	{
		GenCmd(out, "PEXPIREAT", key, t);
	}

	// PTTL key
	// Get the time to live for a key in milliseconds
	void Pttl(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "PTTL", key);
	}

	// RANDOMKEY 
	// Return a random key from the keyspace
	void Randomkey(std::ostream& out)
	{
		GenCmd(out, "RANDOMKEY");
	}

	// RENAME key newkey
	// Rename a key
	void Rename(std::ostream& out, const std::string& key, const std::string& newkey)
	{
		GenCmd(out, "RENAME", key, newkey);
	}

	// RENAMENX key newkey
	// Rename a key, only if the new key does not exist
	void Renamenx(std::ostream& out, const std::string& key, const std::string& newkey)
	{
		GenCmd(out, "RENAMENX", key, newkey);
	}

	// RESTORE key ttl serialized-value [REPLACE]
	// Create a key using the provided serialized value, previously obtained using DUMP.
	void Restore(std::ostream& out)
	{
		// TODO: RESTORE
		assert(false);
	}

	// SORT key [BY pattern] [LIMIT offset count] [GET pattern [GET pattern ...]] [ASC|DESC] [ALPHA] [STORE destination]
	// Sort the elements in a list, set or sorted set
	void Sort(std::ostream& out)
	{
		// TODO: SORT
		assert(false);
	}

	// TOUCH key [key ...]
	// Alters the last access time of a key(s). Returns the number of existing keys specified.
	void Touch(std::ostream& out)
	{
		// TODO: TOUCH
		assert(false);
	}

	// TTL key
	// Get the time to live for a key
	void Ttl(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "TTL", key);
	}

	// TYPE key
	// Determine the type stored at key
	void Type(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "TYPE", key);
	}

	// UNLINK key [key ...]
	// Delete a key asynchronously in another thread. Otherwise it is just as DEL, but non blocking.
	void Unlink(std::ostream& out)
	{
		// TODO: UNLINK
		assert(false);
	}

	// WAIT numslaves timeout
	// Wait for the synchronous replication of all the write commands sent in the context of the current connection
	void Wait(std::ostream& out, int numslaves, int timeout)
	{
		GenCmd(out, "WAIT", numslaves, timeout);
	}

	// SCAN cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate the keys space
	void Scan(std::ostream& out)
	{
		// TODO: SCAN
		assert(false);
	}

} // namespace GenericCommands

#endif // GENERICCOMMANDS_H_
