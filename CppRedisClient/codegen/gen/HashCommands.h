
#ifndef HASHCOMMANDS_H_
#define HASHCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace HashCommands
{

	// HDEL key field [field ...]
	// Delete one or more hash fields
	void Hdel(std::ostream& out)
	{
		// TODO: HDEL
		assert(false);
	}

	// HEXISTS key field
	// Determine if a hash field exists
	void Hexists(std::ostream& out, const std::string& key, const std::string& field)
	{
		GenCmd(out, "HEXISTS", key, field);
	}

	// HGET key field
	// Get the value of a hash field
	void Hget(std::ostream& out, const std::string& key, const std::string& field)
	{
		GenCmd(out, "HGET", key, field);
	}

	// HGETALL key
	// Get all the fields and values in a hash
	void Hgetall(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "HGETALL", key);
	}

	// HINCRBY key field increment
	// Increment the integer value of a hash field by the given number
	void Hincrby(std::ostream& out)
	{
		// TODO: HINCRBY
		assert(false);
	}

	// HINCRBYFLOAT key field increment
	// Increment the float value of a hash field by the given amount
	void Hincrbyfloat(std::ostream& out)
	{
		// TODO: HINCRBYFLOAT
		assert(false);
	}

	// HKEYS key
	// Get all the fields in a hash
	void Hkeys(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "HKEYS", key);
	}

	// HLEN key
	// Get the number of fields in a hash
	void Hlen(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "HLEN", key);
	}

	// HMGET key field [field ...]
	// Get the values of all the given hash fields
	void Hmget(std::ostream& out)
	{
		// TODO: HMGET
		assert(false);
	}

	// HMSET key field value [field value ...]
	// Set multiple hash fields to multiple values
	void Hmset(std::ostream& out)
	{
		// TODO: HMSET
		assert(false);
	}

	// HSET key field value
	// Set the string value of a hash field
	void Hset(std::ostream& out, const std::string& key, const std::string& field, const std::vector<char>& val)
	{
		GenCmd(out, "HSET", key, field, val);
	}

	// HSET key field value
	// Set the string value of a hash field
	void Hset(std::ostream& out, const std::string& key, const std::string& field, const std::string& val)
	{
		GenCmd(out, "HSET", key, field, val);
	}

	// HSETNX key field value
	// Set the value of a hash field, only if the field does not exist
	void Hsetnx(std::ostream& out, const std::string& key, const std::string& field, const std::vector<char>& val)
	{
		GenCmd(out, "HSETNX", key, field, val);
	}

	// HSETNX key field value
	// Set the value of a hash field, only if the field does not exist
	void Hsetnx(std::ostream& out, const std::string& key, const std::string& field, const std::string& val)
	{
		GenCmd(out, "HSETNX", key, field, val);
	}

	// HSTRLEN key field
	// Get the length of the value of a hash field
	void Hstrlen(std::ostream& out, const std::string& key, const std::string& field)
	{
		GenCmd(out, "HSTRLEN", key, field);
	}

	// HVALS key
	// Get all the values in a hash
	void Hvals(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "HVALS", key);
	}

	// HSCAN key cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate hash fields and associated values
	void Hscan(std::ostream& out)
	{
		// TODO: HSCAN
		assert(false);
	}

} // namespace HashCommands

#endif // HASHCOMMANDS_H_
