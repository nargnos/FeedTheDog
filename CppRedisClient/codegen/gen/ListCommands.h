
#ifndef LISTCOMMANDS_H_
#define LISTCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace ListCommands
{

	// BLPOP key [key ...] timeout
	// Remove and get the first element in a list, or block until one is available
	void Blpop(std::ostream& out)
	{
		// TODO: BLPOP
		assert(false);
	}

	// BRPOP key [key ...] timeout
	// Remove and get the last element in a list, or block until one is available
	void Brpop(std::ostream& out)
	{
		// TODO: BRPOP
		assert(false);
	}

	// BRPOPLPUSH source destination timeout
	// Pop a value from a list, push it to another list and return it; or block until one is available
	void Brpoplpush(std::ostream& out)
	{
		// TODO: BRPOPLPUSH
		assert(false);
	}

	// LINDEX key index
	// Get an element from a list by its index
	void Lindex(std::ostream& out, const std::string& key, int index)
	{
		GenCmd(out, "LINDEX", key, index);
	}

	// LINSERT key BEFORE|AFTER pivot value
	// Insert an element before or after another element in a list
	void Linsert(std::ostream& out)
	{
		// TODO: LINSERT
		assert(false);
	}

	// LLEN key
	// Get the length of a list
	void Llen(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "LLEN", key);
	}

	// LPOP key
	// Remove and get the first element in a list
	void Lpop(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "LPOP", key);
	}

	// LPUSH key value [value ...]
	// Prepend one or multiple values to a list
	void Lpush(std::ostream& out)
	{
		// TODO: LPUSH
		assert(false);
	}

	// LPUSHX key value
	// Prepend a value to a list, only if the list exists
	void Lpushx(std::ostream& out, const std::string& key, const std::vector<char>& val)
	{
		GenCmd(out, "LPUSHX", key, val);
	}

	// LPUSHX key value
	// Prepend a value to a list, only if the list exists
	void Lpushx(std::ostream& out, const std::string& key, const std::string& val)
	{
		GenCmd(out, "LPUSHX", key, val);
	}

	// LRANGE key start stop
	// Get a range of elements from a list
	void Lrange(std::ostream& out)
	{
		// TODO: LRANGE
		assert(false);
	}

	// LREM key count value
	// Remove elements from a list
	void Lrem(std::ostream& out)
	{
		// TODO: LREM
		assert(false);
	}

	// LSET key index value
	// Set the value of an element in a list by its index
	void Lset(std::ostream& out)
	{
		// TODO: LSET
		assert(false);
	}

	// LTRIM key start stop
	// Trim a list to the specified range
	void Ltrim(std::ostream& out)
	{
		// TODO: LTRIM
		assert(false);
	}

	// RPOP key
	// Remove and get the last element in a list
	void Rpop(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "RPOP", key);
	}

	// RPOPLPUSH source destination
	// Remove the last element in a list, prepend it to another list and return it
	void Rpoplpush(std::ostream& out)
	{
		// TODO: RPOPLPUSH
		assert(false);
	}

	// RPUSH key value [value ...]
	// Append one or multiple values to a list
	void Rpush(std::ostream& out)
	{
		// TODO: RPUSH
		assert(false);
	}

	// RPUSHX key value
	// Append a value to a list, only if the list exists
	void Rpushx(std::ostream& out, const std::string& key, const std::vector<char>& val)
	{
		GenCmd(out, "RPUSHX", key, val);
	}

	// RPUSHX key value
	// Append a value to a list, only if the list exists
	void Rpushx(std::ostream& out, const std::string& key, const std::string& val)
	{
		GenCmd(out, "RPUSHX", key, val);
	}

} // namespace ListCommands

#endif // LISTCOMMANDS_H_
