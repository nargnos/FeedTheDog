
# ifndef STRINGCOMMANDS_H_
# define STRINGCOMMANDS_H_
# include "Util.h"
namespace StringCommands
{

	// APPEND key value
	// Append a value to a key
	void Append(std::ostream& out, const std::string& key, const std::string& value)
	{
		GenCmd(out, "APPEND", key, value);
	}



	// APPEND key value
	// Append a value to a key
	void Append(std::ostream& out, const std::string& key, const std::vector<char>& value)
	{
		GenCmd(out, "APPEND", key, value);
	}

	// BITCOUNT key [start end]
	// Count set bits in a string
	void Bitcount(std::ostream& out, const std::string& key /*[start end]*/)
	{
		static_assert(false, "not impl");
	}

	// BITFIELD key [GET type offset] [SET type offset value] [INCRBY type offset increment] [OVERFLOW WRAP|SAT|FAIL]
	// Perform arbitrary bitfield integer operations on strings
	void Bitfield(std::ostream& out, const std::string& key /*[GET type offset]*/ /*[SET type offset value]*/ /*[INCRBY type offset increment]*/ /*[OVERFLOW WRAP|SAT|FAIL]*/)
	{
		static_assert(false, "not impl");
	}

	// BITOP operation destkey key [key ...]
	// Perform bitwise operations between strings
	void Bitop(std::ostream& out, Operation operation, const std::string& destkey, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 4 + keys.size(), "BITOP", operation, destkey, key, keys);
	}

	// BITPOS key bit [start] [end]
	// Find first bit set or clear in a string
	void Bitpos(std::ostream& out, const std::string& key, const std::string& bit /*[start]*/ /*[end]*/)
	{
		static_assert(false, "not impl");
	}

	// DECR key
	// Decrement the integer value of a key by one
	void Decr(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "DECR", key);
	}

	// DECRBY key decrement
	// Decrement the integer value of a key by the given number
	void Decrby(std::ostream& out, const std::string& key, int decrement)
	{
		GenCmd(out, "DECRBY", key, decrement);
	}

	// GET key
	// Get the value of a key
	void Get(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "GET", key);
	}

	// GETBIT key offset
	// Returns the bit value at offset in the string value stored at key
	void Getbit(std::ostream& out, const std::string& key, int offset)
	{
		GenCmd(out, "GETBIT", key, offset);
	}

	// GETRANGE key start end
	// Get a substring of the string stored at a key
	void Getrange(std::ostream& out, const std::string& key, int start, int end)
	{
		GenCmd(out, "GETRANGE", key, start, end);
	}

	// GETSET key value
	// Set the string value of a key and return its old value
	void Getset(std::ostream& out, const std::string& key, const std::string& value)
	{
		GenCmd(out, "GETSET", key, value);
	}



	// GETSET key value
	// Set the string value of a key and return its old value
	void Getset(std::ostream& out, const std::string& key, const std::vector<char>& value)
	{
		GenCmd(out, "GETSET", key, value);
	}

	// INCR key
	// Increment the integer value of a key by one
	void Incr(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "INCR", key);
	}

	// INCRBY key increment
	// Increment the integer value of a key by the given amount
	void Incrby(std::ostream& out, const std::string& key, int increment)
	{
		GenCmd(out, "INCRBY", key, increment);
	}

	// INCRBYFLOAT key increment
	// Increment the float value of a key by the given amount
	void Incrbyfloat(std::ostream& out, const std::string& key, float increment)
	{
		GenCmd(out, "INCRBYFLOAT", key, increment);
	}

	// MGET key [key ...]
	// Get the values of all the given keys
	void Mget(std::ostream& out, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 2 + keys.size(), "MGET", key, keys);
	}

	// MSET key value [key value ...]
	// Set multiple keys to multiple values
	void Mset(std::ostream& out, const std::string& key, const std::string& value,
		const std::vector<KeyValuePair>& kvs = {})
	{
		GenCmdWithSize(out, 3 + (kvs.size() << 1), "MSET", key, value, kvs);
	}

	// MSET key value [key value ...]
	// Set multiple keys to multiple values
	void Mset(std::ostream& out, const std::string& key, const std::vector<char>& value,
		const std::vector<KeyVectorPair>& kvs = {})
	{
		GenCmdWithSize(out, 3 + (kvs.size() << 1), "MSET", key, value, kvs);
	}

	// MSETNX key value [key value ...]
	// Set multiple keys to multiple values, only if none of the keys exist
	void Msetnx(std::ostream& out, const std::string& key, const std::string& value,
		const std::vector<KeyValuePair>& kvs = {})
	{
		GenCmdWithSize(out, 3 + (kvs.size() << 1), "MSETNX", key, value, kvs);
	}

	// MSETNX key value [key value ...]
	// Set multiple keys to multiple values, only if none of the keys exist
	void Msetnx(std::ostream& out, const std::string& key, const std::vector<char>& value,
		const std::vector<KeyVectorPair>& kvs = {})
	{
		GenCmdWithSize(out, 3 + (kvs.size() << 1), "MSETNX", key, value, kvs);
	}

	// PSETEX key milliseconds value
	// Set the value and expiration in milliseconds of a key
	void Psetex(std::ostream& out, const std::string& key, std::chrono::milliseconds milliseconds, const std::string& value)
	{
		GenCmd(out, "PSETEX", key, milliseconds, value);
	}



	// PSETEX key milliseconds value
	// Set the value and expiration in milliseconds of a key
	void Psetex(std::ostream& out, const std::string& key, std::chrono::milliseconds milliseconds, const std::vector<char>& value)
	{
		GenCmd(out, "PSETEX", key, milliseconds, value);
	}

	// SET key value [EX seconds] [PX milliseconds] [NX|XX]
	// Set the string value of a key
	void Set(std::ostream& out, const std::string& key, const std::string& value /*[EX seconds]*/ /*[PX milliseconds]*/ /*[NX|XX]*/)
	{
		static_assert(false, "not impl");
	}

	// SET key value [EX seconds] [PX milliseconds] [NX|XX]
	// Set the string value of a key
	void Set(std::ostream& out, const std::string& key, const std::vector<char>& value /*[EX seconds]*/ /*[PX milliseconds]*/ /*[NX|XX]*/)
	{
		static_assert(false, "not impl");
	}

	// SETBIT key offset value
	// Sets or clears the bit at offset in the string value stored at key
	void Setbit(std::ostream& out, const std::string& key, int offset, const std::string& value)
	{
		GenCmd(out, "SETBIT", key, offset, value);
	}



	// SETBIT key offset value
	// Sets or clears the bit at offset in the string value stored at key
	void Setbit(std::ostream& out, const std::string& key, int offset, const std::vector<char>& value)
	{
		GenCmd(out, "SETBIT", key, offset, value);
	}

	// SETEX key seconds value
	// Set the value and expiration of a key
	void Setex(std::ostream& out, const std::string& key, std::chrono::seconds seconds, const std::string& value)
	{
		GenCmd(out, "SETEX", key, seconds, value);
	}



	// SETEX key seconds value
	// Set the value and expiration of a key
	void Setex(std::ostream& out, const std::string& key, std::chrono::seconds seconds, const std::vector<char>& value)
	{
		GenCmd(out, "SETEX", key, seconds, value);
	}

	// SETNX key value
	// Set the value of a key, only if the key does not exist
	void Setnx(std::ostream& out, const std::string& key, const std::string& value)
	{
		GenCmd(out, "SETNX", key, value);
	}



	// SETNX key value
	// Set the value of a key, only if the key does not exist
	void Setnx(std::ostream& out, const std::string& key, const std::vector<char>& value)
	{
		GenCmd(out, "SETNX", key, value);
	}

	// SETRANGE key offset value
	// Overwrite part of a string at key starting at the specified offset
	void Setrange(std::ostream& out, const std::string& key, int offset, const std::string& value)
	{
		GenCmd(out, "SETRANGE", key, offset, value);
	}



	// SETRANGE key offset value
	// Overwrite part of a string at key starting at the specified offset
	void Setrange(std::ostream& out, const std::string& key, int offset, const std::vector<char>& value)
	{
		GenCmd(out, "SETRANGE", key, offset, value);
	}

	// STRLEN key
	// Get the length of the value stored in a key
	void Strlen(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "STRLEN", key);
	}

} // namespace StringCommands

# endif // STRINGCOMMANDS_H_
