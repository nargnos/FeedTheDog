
# ifndef LISTCOMMANDS_H_
# define LISTCOMMANDS_H_
# include "Util.h"
namespace ListCommands
{

	// BLPOP key [key ...] timeout
	// Remove and get the first element in a list, or block until one is available
	void Blpop(std::ostream& out, const std::string& key, int timeout)
	{
		GenCmd(out, "BLPOP", key, timeout);
	}

	// BLPOP key [key ...] timeout
	// Remove and get the first element in a list, or block until one is available
	void Blpop(std::ostream& out, const std::string& key, const std::vector<std::string>& keys, int timeout)
	{
		GenCmdWithSize(out, 3 + keys.size(), "BLPOP", key, keys, timeout);
	}

	// BRPOP key [key ...] timeout
	// Remove and get the last element in a list, or block until one is available
	void Brpop(std::ostream& out, const std::string& key, int timeout)
	{
		GenCmd(out, "BRPOP", key, timeout);
	}

	// BRPOP key [key ...] timeout
	// Remove and get the last element in a list, or block until one is available
	void Brpop(std::ostream& out, const std::string& key, const std::vector<std::string>& keys, int timeout)
	{
		GenCmdWithSize(out, 3 + keys.size(), "BRPOP", key, keys, timeout);
	}

	// BRPOPLPUSH source destination timeout
	// Pop a value from a list, push it to another list and return it; or block until one is available
	void Brpoplpush(std::ostream& out, const std::string& source, const std::string& destination, int timeout)
	{
		GenCmd(out, "BRPOPLPUSH", source, destination, timeout);
	}

	// LINDEX key index
	// Get an element from a list by its index
	void Lindex(std::ostream& out, const std::string& key, int index)
	{
		GenCmd(out, "LINDEX", key, index);
	}

	// LINSERT key BEFORE|AFTER pivot value
	// Insert an element before or after another element in a list
	void Linsert(std::ostream& out, const std::string& key, InsertPos beforeAfter,
		const std::string& pivot, const std::string& value)
	{
		GenCmd(out, "LINSERT", key, beforeAfter, pivot, value);
	}



	// LINSERT key BEFORE|AFTER pivot value
	// Insert an element before or after another element in a list
	void Linsert(std::ostream& out, const std::string& key, InsertPos beforeAfter,
		const std::string& pivot, const std::vector<char>& value)
	{
		GenCmd(out, "LINSERT", key, beforeAfter, pivot, value);
	}



	// LINSERT key BEFORE|AFTER pivot value
	// Insert an element before or after another element in a list
	void Linsert(std::ostream& out, const std::string& key, InsertPos beforeAfter,
		const std::vector<char>& pivot, const std::string& value)
	{
		GenCmd(out, "LINSERT", key, beforeAfter, pivot, value);
	}



	// LINSERT key BEFORE|AFTER pivot value
	// Insert an element before or after another element in a list
	void Linsert(std::ostream& out, const std::string& key, InsertPos beforeAfter,
		const std::vector<char>& pivot, const std::vector<char>& value)
	{
		GenCmd(out, "LINSERT", key, beforeAfter, pivot, value);
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
	void Lpush(std::ostream& out, const std::string& key,
		const std::string& value, const std::vector<std::string>& values = {})
	{
		GenCmdWithSize(out, 3 + values.size(), "LPUSH", key, value, values);
	}

	// LPUSH key value [value ...]
	// Prepend one or multiple values to a list
	void Lpush(std::ostream& out, const std::string& key,
		const std::vector<char>& value, const std::vector<std::vector<char>>& values = {})
	{
		GenCmdWithSize(out, 3 + values.size(), "LPUSH", key, value, values);
	}

	// LPUSHX key value
	// Prepend a value to a list, only if the list exists
	void Lpushx(std::ostream& out, const std::string& key, const std::string& value)
	{
		GenCmd(out, "LPUSHX", key, value);
	}



	// LPUSHX key value
	// Prepend a value to a list, only if the list exists
	void Lpushx(std::ostream& out, const std::string& key, const std::vector<char>& value)
	{
		GenCmd(out, "LPUSHX", key, value);
	}

	// LRANGE key start stop
	// Get a range of elements from a list
	void Lrange(std::ostream& out, const std::string& key, int start, int stop)
	{
		GenCmd(out, "LRANGE", key, start, stop);
	}

	// LREM key count value
	// Remove elements from a list
	void Lrem(std::ostream& out, const std::string& key, int count, const std::string& value)
	{
		GenCmd(out, "LREM", key, count, value);
	}



	// LREM key count value
	// Remove elements from a list
	void Lrem(std::ostream& out, const std::string& key, int count, const std::vector<char>& value)
	{
		GenCmd(out, "LREM", key, count, value);
	}

	// LSET key index value
	// Set the value of an element in a list by its index
	void Lset(std::ostream& out, const std::string& key, int index, const std::string& value)
	{
		GenCmd(out, "LSET", key, index, value);
	}



	// LSET key index value
	// Set the value of an element in a list by its index
	void Lset(std::ostream& out, const std::string& key, int index, const std::vector<char>& value)
	{
		GenCmd(out, "LSET", key, index, value);
	}

	// LTRIM key start stop
	// Trim a list to the specified range
	void Ltrim(std::ostream& out, const std::string& key, int start, int stop)
	{
		GenCmd(out, "LTRIM", key, start, stop);
	}

	// RPOP key
	// Remove and get the last element in a list
	void Rpop(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "RPOP", key);
	}

	// RPOPLPUSH source destination
	// Remove the last element in a list, prepend it to another list and return it
	void Rpoplpush(std::ostream& out, const std::string& source, const std::string& destination)
	{
		GenCmd(out, "RPOPLPUSH", source, destination);
	}

	// RPUSH key value [value ...]
	// Append one or multiple values to a list
	void Rpush(std::ostream& out, const std::string& key,
		const std::string& value, const std::vector<std::string>& values = {})
	{
		GenCmdWithSize(out, 3 + values.size(), "RPUSH", key, value, values);
	}

	// RPUSH key value [value ...]
	// Append one or multiple values to a list
	void Rpush(std::ostream& out, const std::string& key,
		const std::vector<char>& value, const std::vector<std::vector<char>>& values = {})
	{
		GenCmdWithSize(out, 3 + values.size(), "RPUSH", key, value, values);
	}

	// RPUSHX key value
	// Append a value to a list, only if the list exists
	void Rpushx(std::ostream& out, const std::string& key, const std::string& value)
	{
		GenCmd(out, "RPUSHX", key, value);
	}



	// RPUSHX key value
	// Append a value to a list, only if the list exists
	void Rpushx(std::ostream& out, const std::string& key, const std::vector<char>& value)
	{
		GenCmd(out, "RPUSHX", key, value);
	}

} // namespace ListCommands

# endif // LISTCOMMANDS_H_
