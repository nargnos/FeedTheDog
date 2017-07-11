
# ifndef HASHCOMMANDS_H_
# define HASHCOMMANDS_H_
# include "Util.h"
namespace HashCommands
{

	// HDEL key field [field ...]
	// Delete one or more hash fields
	void Hdel(std::ostream& out, const std::string& key, const std::string& field,
		const std::vector<std::string>& fields = {})
	{
		GenCmdWithSize(out, 3 + fields.size(), "HDEL", key, field, fields);
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
	void Hincrby(std::ostream& out, const std::string& key, const std::string& field, int increment)
	{
		GenCmd(out, "HINCRBY", key, field, increment);
	}

	// HINCRBYFLOAT key field increment
	// Increment the float value of a hash field by the given amount
	void Hincrbyfloat(std::ostream& out, const std::string& key, const std::string& field, int increment)
	{
		GenCmd(out, "HINCRBYFLOAT", key, field, increment);
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
	void Hmget(std::ostream& out, const std::string& key, const std::string& field,
		const std::vector<std::string>& fields = {})
	{
		GenCmdWithSize(out, 3 + fields.size(), "HMGET", key, field, fields);
	}

	// HMSET key field value [field value ...]
	// Set multiple hash fields to multiple values
	void Hmset(std::ostream& out, const std::string& key, const std::string& field, const std::string& value,
		const std::vector<KeyValuePair>& kvs = {})
	{
		GenCmdWithSize(out, 4 + (kvs.size() << 1), "HMSET", key, field, value, kvs);
	}

	// HMSET key field value [field value ...]
	// Set multiple hash fields to multiple values
	void Hmset(std::ostream& out, const std::string& key, const std::string& field, const std::vector<char>& value,
		const std::vector<KeyVectorPair>& kvs = {})
	{
		GenCmdWithSize(out, 4 + (kvs.size() << 1), "HMSET", key, field, value, kvs);
	}

	// HSET key field value
	// Set the string value of a hash field
	void Hset(std::ostream& out, const std::string& key, const std::string& field, const std::string& value)
	{
		GenCmd(out, "HSET", key, field, value);
	}



	// HSET key field value
	// Set the string value of a hash field
	void Hset(std::ostream& out, const std::string& key, const std::string& field, const std::vector<char>& value)
	{
		GenCmd(out, "HSET", key, field, value);
	}

	// HSETNX key field value
	// Set the value of a hash field, only if the field does not exist
	void Hsetnx(std::ostream& out, const std::string& key, const std::string& field, const std::string& value)
	{
		GenCmd(out, "HSETNX", key, field, value);
	}



	// HSETNX key field value
	// Set the value of a hash field, only if the field does not exist
	void Hsetnx(std::ostream& out, const std::string& key, const std::string& field, const std::vector<char>& value)
	{
		GenCmd(out, "HSETNX", key, field, value);
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
	void Hscan(std::ostream& out, const std::string& key, int cursor, const std::string& pattern, int count)
	{
		GenCmd(out, "HSCAN", key, cursor, "MATCH", pattern, "COUNT", count);
	}

	// HSCAN key cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate hash fields and associated values
	void Hscan(std::ostream& out, const std::string& key, int cursor, int count)
	{
		GenCmd(out, "HSCAN", key, cursor, "COUNT", count);
	}

	// HSCAN key cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate hash fields and associated values
	void Hscan(std::ostream& out, const std::string& key, int cursor, const std::string& pattern)
	{
		GenCmd(out, "HSCAN", key, cursor, "MATCH", pattern);
	}
} // namespace HashCommands

# endif // HASHCOMMANDS_H_
