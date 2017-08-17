
# ifndef SETCOMMANDS_H_
# define SETCOMMANDS_H_
# include "Util.h"
namespace SetCommands
{

	// SADD key member [member ...]
	// Add one or more members to a set
	void Sadd(std::ostream& out, const std::string& key, const std::string& member,
		const std::vector<std::string>& members = {})
	{
		GenCmdWithSize(out, 3 + members.size(), "SADD", key, member, members);
	}

	// SCARD key
	// Get the number of members in a set
	void Scard(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "SCARD", key);
	}

	// SDIFF key [key ...]
	// Subtract multiple sets
	void Sdiff(std::ostream& out, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 2 + keys.size(), "SDIFF", key, keys);
	}

	// SDIFFSTORE destination key [key ...]
	// Subtract multiple sets and store the resulting set in a key
	void Sdiffstore(std::ostream& out, const std::string& destination, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 3 + keys.size(), "SDIFFSTORE", destination, key, keys);
	}

	// SINTER key [key ...]
	// Intersect multiple sets
	void Sinter(std::ostream& out, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 2 + keys.size(), "SINTER", key, keys);
	}

	// SINTERSTORE destination key [key ...]
	// Intersect multiple sets and store the resulting set in a key
	void Sinterstore(std::ostream& out, const std::string& destination, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 3 + keys.size(), "SINTERSTORE", destination, key, keys);
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
	void Smove(std::ostream& out, const std::string& source,
		const std::string& destination, const std::string& member)
	{
		GenCmd(out, "SMOVE", source, destination, member);
	}

	// SPOP key [count]
	// Remove and return one or multiple random members from a set
	void Spop(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "SPOP", key);
	}

	// SPOP key [count]
	// Remove and return one or multiple random members from a set
	void Spop(std::ostream& out, const std::string& key, int count)
	{
		GenCmd(out, "SPOP", key, count);
	}

	// SRANDMEMBER key [count]
	// Get one or multiple random members from a set
	void Srandmember(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "SRANDMEMBER", key);
	}

	// SRANDMEMBER key [count]
	// Get one or multiple random members from a set
	void Srandmember(std::ostream& out, const std::string& key, int count)
	{
		GenCmd(out, "SRANDMEMBER", key, count);
	}

	// SREM key member [member ...]
	// Remove one or more members from a set
	void Srem(std::ostream& out, const std::string& key, const std::string& member,
		const std::vector<std::string>& members = {})
	{
		GenCmdWithSize(out, 3 + members.size(), "SREM", key, member, members);
	}

	// SUNION key [key ...]
	// Add multiple sets
	void Sunion(std::ostream& out, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 2 + keys.size(), "SUNION", key, keys);
	}

	// SUNIONSTORE destination key [key ...]
	// Add multiple sets and store the resulting set in a key
	void Sunionstore(std::ostream& out, const std::string& destination, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 3 + keys.size(), "SUNION", key, keys);
	}

	// SSCAN key cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate Set elements
	void Sscan(std::ostream& out, const std::string& key, int cursor)
	{
		GenCmd(out, "SSCAN", key, cursor);
	}

	// SSCAN key cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate Set elements
	void Sscan(std::ostream& out, const std::string& key, int cursor, const std::string& pattern)
	{
		GenCmd(out, "SSCAN", key, cursor, "MATCH", pattern);
	}

	// SSCAN key cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate Set elements
	void Sscan(std::ostream& out, const std::string& key, int cursor, const std::string& pattern, int count)
	{
		GenCmd(out, "SSCAN", key, cursor, "MATCH", pattern, "COUNT", count);
	}

	// SSCAN key cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate Set elements
	void Sscan(std::ostream& out, const std::string& key, int cursor, int count)
	{
		GenCmd(out, "SSCAN", key, cursor, "COUNT", count);
	}
} // namespace SetCommands

# endif // SETCOMMANDS_H_
