
#ifndef STRINGCOMMANDS_H_
#define STRINGCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace StringCommands
{

	// APPEND key value
	// Append a value to a key
	void Append(std::ostream& out, const std::string& key, const std::vector<char>& val)
	{
		GenCmd(out, "APPEND", key, val);
	}

	// APPEND key value
	// Append a value to a key
	void Append(std::ostream& out, const std::string& key, const std::string& val)
	{
		GenCmd(out, "APPEND", key, val);
	}

	// BITCOUNT key [start end]
	// Count set bits in a string
	void Bitcount(std::ostream& out)
	{
		// TODO: BITCOUNT
		assert(false);
	}

	// BITFIELD key [GET type offset] [SET type offset value] [INCRBY type offset increment] [OVERFLOW WRAP|SAT|FAIL]
	// Perform arbitrary bitfield integer operations on strings
	void Bitfield(std::ostream& out)
	{
		// TODO: BITFIELD
		assert(false);
	}

	// BITOP operation destkey key [key ...]
	// Perform bitwise operations between strings
	void Bitop(std::ostream& out)
	{
		// TODO: BITOP
		assert(false);
	}

	// BITPOS key bit [start] [end]
	// Find first bit set or clear in a string
	void Bitpos(std::ostream& out)
	{
		// TODO: BITPOS
		assert(false);
	}

	// DECR key
	// Decrement the integer value of a key by one
	void Decr(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "DECR", key);
	}

	// DECRBY key decrement
	// Decrement the integer value of a key by the given number
	void Decrby(std::ostream& out, const std::string& key, int d)
	{
		GenCmd(out, "DECRBY", key, d);
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
	void Getset(std::ostream& out, const std::string& key, const std::vector<char>& val)
	{
		GenCmd(out, "GETSET", key, val);
	}

	// GETSET key value
	// Set the string value of a key and return its old value
	void Getset(std::ostream& out, const std::string& key, const std::string& val)
	{
		GenCmd(out, "GETSET", key, val);
	}

	// INCR key
	// Increment the integer value of a key by one
	void Incr(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "INCR", key);
	}

	// INCRBY key increment
	// Increment the integer value of a key by the given amount
	void Incrby(std::ostream& out, const std::string& key, int i)
	{
		GenCmd(out, "INCRBY", key, i);
	}

	// INCRBYFLOAT key incrementFloat
	// Increment the float value of a key by the given amount
	void Incrbyfloat(std::ostream& out, const std::string& key, float i)
	{
		GenCmd(out, "INCRBYFLOAT", key, i);
	}

	// MGET key [key ...]
	// Get the values of all the given keys
	void Mget(std::ostream& out)
	{
		// TODO: MGET
		assert(false);
	}

	// MSET key value [key value ...]
	// Set multiple keys to multiple values
	void Mset(std::ostream& out)
	{
		// TODO: MSET
		assert(false);
	}

	// MSETNX key value [key value ...]
	// Set multiple keys to multiple values, only if none of the keys exist
	void Msetnx(std::ostream& out)
	{
		// TODO: MSETNX
		assert(false);
	}

	// PSETEX key milliseconds value
	// Set the value and expiration in milliseconds of a key
	void Psetex(std::ostream& out, const std::string& key, int ms, const std::vector<char>& val)
	{
		GenCmd(out, "PSETEX", key, ms, val);
	}

	// PSETEX key milliseconds value
	// Set the value and expiration in milliseconds of a key
	void Psetex(std::ostream& out, const std::string& key, int ms, const std::string& val)
	{
		GenCmd(out, "PSETEX", key, ms, val);
	}

	// SET key value [EX seconds] [PX milliseconds] [NX|XX]
	// Set the string value of a key
	void Set(std::ostream& out)
	{
		// TODO: SET
		assert(false);
	}

	// SETBIT key offset value
	// Sets or clears the bit at offset in the string value stored at key
	void Setbit(std::ostream& out, const std::string& key, int offset, const std::vector<char>& val)
	{
		GenCmd(out, "SETBIT", offset, offset, val);
	}

	// SETBIT key offset value
	// Sets or clears the bit at offset in the string value stored at key
	void Setbit(std::ostream& out, const std::string& key, int offset, const std::string& val)
	{
		GenCmd(out, "SETBIT", offset, offset, val);
	}

	// SETEX key seconds value
	// Set the value and expiration of a key
	void Setex(std::ostream& out, const std::string& key, int s, const std::vector<char>& val)
	{
		GenCmd(out, "SETEX", s, val);
	}

	// SETEX key seconds value
	// Set the value and expiration of a key
	void Setex(std::ostream& out, const std::string& key, int s, const std::string& val)
	{
		GenCmd(out, "SETEX", s, val);
	}

	// SETNX key value
	// Set the value of a key, only if the key does not exist
	void Setnx(std::ostream& out, const std::string& key, const std::vector<char>& val)
	{
		GenCmd(out, "SETNX", key, val);
	}

	// SETNX key value
	// Set the value of a key, only if the key does not exist
	void Setnx(std::ostream& out, const std::string& key, const std::string& val)
	{
		GenCmd(out, "SETNX", key, val);
	}

	// SETRANGE key offset value
	// Overwrite part of a string at key starting at the specified offset
	void Setrange(std::ostream& out, const std::string& key, int offset, const std::vector<char>& val)
	{
		GenCmd(out, "SETRANGE", offset, offset, val);
	}

	// SETRANGE key offset value
	// Overwrite part of a string at key starting at the specified offset
	void Setrange(std::ostream& out, const std::string& key, int offset, const std::string& val)
	{
		GenCmd(out, "SETRANGE", offset, offset, val);
	}

	// STRLEN key
	// Get the length of the value stored in a key
	void Strlen(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "STRLEN", key);
	}

} // namespace StringCommands

#endif // STRINGCOMMANDS_H_
