
#ifndef SORTEDSETCOMMANDS_H_
#define SORTEDSETCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace SortedSetCommands
{

	// ZADD key [NX|XX] [CH] [INCR] score member [score member ...]
	// Add one or more members to a sorted set, or update its score if it already exists
	void Zadd(std::ostream& out)
	{
		// TODO: ZADD
		assert(false);
	}

	// ZCARD key
	// Get the number of members in a sorted set
	void Zcard(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "ZCARD", key);
	}

	// ZCOUNT key min max
	// Count the members in a sorted set with scores within the given values
	void Zcount(std::ostream& out, const std::string& key, int min, int max)
	{
		GenCmd(out, "ZCOUNT", key, min, max);
	}

	// ZINCRBY key increment member
	// Increment the score of a member in a sorted set
	void Zincrby(std::ostream& out)
	{
		// TODO: ZINCRBY
		assert(false);
	}

	// ZINTERSTORE destination numkeys key [key ...] [WEIGHTS weight [weight ...]] [AGGREGATE SUM|MIN|MAX]
	// Intersect multiple sorted sets and store the resulting sorted set in a new key
	void Zinterstore(std::ostream& out)
	{
		// TODO: ZINTERSTORE
		assert(false);
	}

	// ZLEXCOUNT key min max
	// Count the number of members in a sorted set between a given lexicographical range
	void Zlexcount(std::ostream& out, const std::string& key, int min, int max)
	{
		GenCmd(out, "ZLEXCOUNT", key, min, max);
	}

	// ZRANGE key start stop [WITHSCORES]
	// Return a range of members in a sorted set, by index
	void Zrange(std::ostream& out)
	{
		// TODO: ZRANGE
		assert(false);
	}

	// ZRANGEBYLEX key min max [LIMIT offset count]
	// Return a range of members in a sorted set, by lexicographical range
	void Zrangebylex(std::ostream& out)
	{
		// TODO: ZRANGEBYLEX
		assert(false);
	}

	// ZREVRANGEBYLEX key max min [LIMIT offset count]
	// Return a range of members in a sorted set, by lexicographical range, ordered from higher to lower strings.
	void Zrevrangebylex(std::ostream& out)
	{
		// TODO: ZREVRANGEBYLEX
		assert(false);
	}

	// ZRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]
	// Return a range of members in a sorted set, by score
	void Zrangebyscore(std::ostream& out)
	{
		// TODO: ZRANGEBYSCORE
		assert(false);
	}

	// ZRANK key member
	// Determine the index of a member in a sorted set
	void Zrank(std::ostream& out, const std::string& key, const std::string& member)
	{
		GenCmd(out, "ZRANK", key, member);
	}

	// ZREM key member [member ...]
	// Remove one or more members from a sorted set
	void Zrem(std::ostream& out)
	{
		// TODO: ZREM
		assert(false);
	}

	// ZREMRANGEBYLEX key min max
	// Remove all members in a sorted set between the given lexicographical range
	void Zremrangebylex(std::ostream& out, const std::string& key, int min, int max)
	{
		GenCmd(out, "ZREMRANGEBYLEX", key, min, max);
	}

	// ZREMRANGEBYRANK key start stop
	// Remove all members in a sorted set within the given indexes
	void Zremrangebyrank(std::ostream& out)
	{
		// TODO: ZREMRANGEBYRANK
		assert(false);
	}

	// ZREMRANGEBYSCORE key min max
	// Remove all members in a sorted set within the given scores
	void Zremrangebyscore(std::ostream& out, const std::string& key, int min, int max)
	{
		GenCmd(out, "ZREMRANGEBYSCORE", key, min, max);
	}

	// ZREVRANGE key start stop [WITHSCORES]
	// Return a range of members in a sorted set, by index, with scores ordered from high to low
	void Zrevrange(std::ostream& out)
	{
		// TODO: ZREVRANGE
		assert(false);
	}

	// ZREVRANGEBYSCORE key max min [WITHSCORES] [LIMIT offset count]
	// Return a range of members in a sorted set, by score, with scores ordered from high to low
	void Zrevrangebyscore(std::ostream& out)
	{
		// TODO: ZREVRANGEBYSCORE
		assert(false);
	}

	// ZREVRANK key member
	// Determine the index of a member in a sorted set, with scores ordered from high to low
	void Zrevrank(std::ostream& out, const std::string& key, const std::string& member)
	{
		GenCmd(out, "ZREVRANK", key, member);
	}

	// ZSCORE key member
	// Get the score associated with the given member in a sorted set
	void Zscore(std::ostream& out, const std::string& key, const std::string& member)
	{
		GenCmd(out, "ZSCORE", key, member);
	}

	// ZUNIONSTORE destination numkeys key [key ...] [WEIGHTS weight [weight ...]] [AGGREGATE SUM|MIN|MAX]
	// Add multiple sorted sets and store the resulting sorted set in a new key
	void Zunionstore(std::ostream& out)
	{
		// TODO: ZUNIONSTORE
		assert(false);
	}

	// ZSCAN key cursor [MATCH pattern] [COUNT count]
	// Incrementally iterate sorted sets elements and associated scores
	void Zscan(std::ostream& out)
	{
		// TODO: ZSCAN
		assert(false);
	}

} // namespace SortedSetCommands

#endif // SORTEDSETCOMMANDS_H_
