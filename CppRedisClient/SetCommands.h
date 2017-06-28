
#ifndef SETCOMMANDS_H_
#define SETCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace SetCommands
{

    // SADD key member [member ...]
    // Add one or more members to a set
	void Sadd(std::ostream& out)
	{
        // TODO: SADD
		assert(false);
	}

    // SCARD key
    // Get the number of members in a set
	void Scard(std::ostream& out, const std::string& key)
	{
        GenCmd(out, "SCARD", key);
	}

    // SDIFF key [key ...]
    // Subtract multiple sets
	void Sdiff(std::ostream& out)
	{
        // TODO: SDIFF
		assert(false);
	}

    // SDIFFSTORE destination key [key ...]
    // Subtract multiple sets and store the resulting set in a key
	void Sdiffstore(std::ostream& out)
	{
        // TODO: SDIFFSTORE
		assert(false);
	}

    // SINTER key [key ...]
    // Intersect multiple sets
	void Sinter(std::ostream& out)
	{
        // TODO: SINTER
		assert(false);
	}

    // SINTERSTORE destination key [key ...]
    // Intersect multiple sets and store the resulting set in a key
	void Sinterstore(std::ostream& out)
	{
        // TODO: SINTERSTORE
		assert(false);
	}

    // SISMEMBER key member
    // Determine if a given value is a member of a set
	void Sismember(std::ostream& out, const std::string& key, const std::string& member)
	{
        GenCmd(out, "SISMEMBER", key, member);
	}

    // SMEMBERS key
    // Get all the members in a set
	void Smembers(std::ostream& out, const std::string& key)
	{
        GenCmd(out, "SMEMBERS", key);
	}

    // SMOVE source destination member
    // Move a member from one set to another
	void Smove(std::ostream& out)
	{
        // TODO: SMOVE
		assert(false);
	}

    // SPOP key [count]
    // Remove and return one or multiple random members from a set
	void Spop(std::ostream& out)
	{
        // TODO: SPOP
		assert(false);
	}

    // SRANDMEMBER key [count]
    // Get one or multiple random members from a set
	void Srandmember(std::ostream& out)
	{
        // TODO: SRANDMEMBER
		assert(false);
	}

    // SREM key member [member ...]
    // Remove one or more members from a set
	void Srem(std::ostream& out)
	{
        // TODO: SREM
		assert(false);
	}

    // SUNION key [key ...]
    // Add multiple sets
	void Sunion(std::ostream& out)
	{
        // TODO: SUNION
		assert(false);
	}

    // SUNIONSTORE destination key [key ...]
    // Add multiple sets and store the resulting set in a key
	void Sunionstore(std::ostream& out)
	{
        // TODO: SUNIONSTORE
		assert(false);
	}

    // SSCAN key cursor [MATCH pattern] [COUNT count]
    // Incrementally iterate Set elements
	void Sscan(std::ostream& out)
	{
        // TODO: SSCAN
		assert(false);
	}

} // namespace SetCommands

#endif // SETCOMMANDS_H_
