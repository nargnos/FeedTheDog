
# ifndef SORTEDSETCOMMANDS_H_
# define SORTEDSETCOMMANDS_H_
# include "Util.h"
namespace SortedSetCommands
{

    // ZADD key [NX|XX] [CH] [INCR] score member [score member ...]
    // Add one or more members to a sorted set, or update its score if it already exists
    void Zadd(std::ostream& out, const std::string& key /*[NX|XX]*/ /*[CH]*/ /*[INCR]*/, const std::string& score, const std::string& member /*[score member ...]*/)
    {
        static_assert(false,"not impl");
    }

    // ZADD key [NX|XX] [CH] [INCR] score member [score member ...]
    // Add one or more members to a sorted set, or update its score if it already exists
    void Zadd(std::ostream& out, const std::string& key /*[NX|XX]*/ /*[CH]*/ /*[INCR]*/, int score, const std::string& member /*[score member ...]*/)
    {
        static_assert(false,"not impl");
    }

    // ZCARD key
    // Get the number of members in a sorted set
    void Zcard(std::ostream& out, const std::string& key)
    {
        GenCmd(out, "ZCARD", key);
    }

    // ZCOUNT key min max
    // Count the members in a sorted set with scores within the given values
    void Zcount(std::ostream& out, const std::string& key, const std::string& min, const std::string& max)
    {
        GenCmd(out, "ZCOUNT", key, min, max);
    }



    // ZCOUNT key min max
    // Count the members in a sorted set with scores within the given values
    void Zcount(std::ostream& out, const std::string& key, const std::string& min, int max)
    {
        GenCmd(out, "ZCOUNT", key, min, max);
    }



    // ZCOUNT key min max
    // Count the members in a sorted set with scores within the given values
    void Zcount(std::ostream& out, const std::string& key, int min, const std::string& max)
    {
        GenCmd(out, "ZCOUNT", key, min, max);
    }



    // ZCOUNT key min max
    // Count the members in a sorted set with scores within the given values
    void Zcount(std::ostream& out, const std::string& key, int min, int max)
    {
        GenCmd(out, "ZCOUNT", key, min, max);
    }

    // ZINCRBY key increment member
    // Increment the score of a member in a sorted set
    void Zincrby(std::ostream& out, const std::string& key, int increment, const std::string& member)
    {
        GenCmd(out, "ZINCRBY", key, increment, member);
    }

    // ZINTERSTORE destination numkeys key [key ...] [WEIGHTS weight [weight ...]] [AGGREGATE SUM|MIN|MAX]
    // Intersect multiple sorted sets and store the resulting sorted set in a new key
    void Zinterstore(std::ostream& out, const std::string& destination, size_t numkeys, const std::string& key /*[key ...]*/ /*[WEIGHTS weight [weight ...]]*/ /*[AGGREGATE SUM|MIN|MAX]*/)
    {
        static_assert(false,"not impl");
    }

    // ZLEXCOUNT key min max
    // Count the number of members in a sorted set between a given lexicographical range
    void Zlexcount(std::ostream& out, const std::string& key, const std::string& min, const std::string& max)
    {
        GenCmd(out, "ZLEXCOUNT", key, min, max);
    }



    // ZLEXCOUNT key min max
    // Count the number of members in a sorted set between a given lexicographical range
    void Zlexcount(std::ostream& out, const std::string& key, const std::string& min, int max)
    {
        GenCmd(out, "ZLEXCOUNT", key, min, max);
    }



    // ZLEXCOUNT key min max
    // Count the number of members in a sorted set between a given lexicographical range
    void Zlexcount(std::ostream& out, const std::string& key, int min, const std::string& max)
    {
        GenCmd(out, "ZLEXCOUNT", key, min, max);
    }



    // ZLEXCOUNT key min max
    // Count the number of members in a sorted set between a given lexicographical range
    void Zlexcount(std::ostream& out, const std::string& key, int min, int max)
    {
        GenCmd(out, "ZLEXCOUNT", key, min, max);
    }

    // ZRANGE key start stop [WITHSCORES]
    // Return a range of members in a sorted set, by index
    void Zrange(std::ostream& out, const std::string& key, int start, int stop /*[WITHSCORES]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANGEBYLEX key min max [LIMIT offset count]
    // Return a range of members in a sorted set, by lexicographical range
    void Zrangebylex(std::ostream& out, const std::string& key, const std::string& min, const std::string& max /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANGEBYLEX key min max [LIMIT offset count]
    // Return a range of members in a sorted set, by lexicographical range
    void Zrangebylex(std::ostream& out, const std::string& key, const std::string& min, int max /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANGEBYLEX key min max [LIMIT offset count]
    // Return a range of members in a sorted set, by lexicographical range
    void Zrangebylex(std::ostream& out, const std::string& key, int min, const std::string& max /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANGEBYLEX key min max [LIMIT offset count]
    // Return a range of members in a sorted set, by lexicographical range
    void Zrangebylex(std::ostream& out, const std::string& key, int min, int max /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREVRANGEBYLEX key max min [LIMIT offset count]
    // Return a range of members in a sorted set, by lexicographical range, ordered from higher to lower strings.
    void Zrevrangebylex(std::ostream& out, const std::string& key, const std::string& max, const std::string& min /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREVRANGEBYLEX key max min [LIMIT offset count]
    // Return a range of members in a sorted set, by lexicographical range, ordered from higher to lower strings.
    void Zrevrangebylex(std::ostream& out, const std::string& key, const std::string& max, int min /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREVRANGEBYLEX key max min [LIMIT offset count]
    // Return a range of members in a sorted set, by lexicographical range, ordered from higher to lower strings.
    void Zrevrangebylex(std::ostream& out, const std::string& key, int max, const std::string& min /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREVRANGEBYLEX key max min [LIMIT offset count]
    // Return a range of members in a sorted set, by lexicographical range, ordered from higher to lower strings.
    void Zrevrangebylex(std::ostream& out, const std::string& key, int max, int min /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]
    // Return a range of members in a sorted set, by score
    void Zrangebyscore(std::ostream& out, const std::string& key, const std::string& min, const std::string& max /*[WITHSCORES]*/ /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]
    // Return a range of members in a sorted set, by score
    void Zrangebyscore(std::ostream& out, const std::string& key, const std::string& min, int max /*[WITHSCORES]*/ /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]
    // Return a range of members in a sorted set, by score
    void Zrangebyscore(std::ostream& out, const std::string& key, int min, const std::string& max /*[WITHSCORES]*/ /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]
    // Return a range of members in a sorted set, by score
    void Zrangebyscore(std::ostream& out, const std::string& key, int min, int max /*[WITHSCORES]*/ /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZRANK key member
    // Determine the index of a member in a sorted set
    void Zrank(std::ostream& out, const std::string& key, const std::string& member)
    {
        GenCmd(out, "ZRANK", key, member);
    }

    // ZREM key member [member ...]
    // Remove one or more members from a sorted set
    void Zrem(std::ostream& out, const std::string& key, const std::string& member /*[member ...]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREMRANGEBYLEX key min max
    // Remove all members in a sorted set between the given lexicographical range
    void Zremrangebylex(std::ostream& out, const std::string& key, const std::string& min, const std::string& max)
    {
        GenCmd(out, "ZREMRANGEBYLEX", key, min, max);
    }



    // ZREMRANGEBYLEX key min max
    // Remove all members in a sorted set between the given lexicographical range
    void Zremrangebylex(std::ostream& out, const std::string& key, const std::string& min, int max)
    {
        GenCmd(out, "ZREMRANGEBYLEX", key, min, max);
    }



    // ZREMRANGEBYLEX key min max
    // Remove all members in a sorted set between the given lexicographical range
    void Zremrangebylex(std::ostream& out, const std::string& key, int min, const std::string& max)
    {
        GenCmd(out, "ZREMRANGEBYLEX", key, min, max);
    }



    // ZREMRANGEBYLEX key min max
    // Remove all members in a sorted set between the given lexicographical range
    void Zremrangebylex(std::ostream& out, const std::string& key, int min, int max)
    {
        GenCmd(out, "ZREMRANGEBYLEX", key, min, max);
    }

    // ZREMRANGEBYRANK key start stop
    // Remove all members in a sorted set within the given indexes
    void Zremrangebyrank(std::ostream& out, const std::string& key, int start, int stop)
    {
        GenCmd(out, "ZREMRANGEBYRANK", key, start, stop);
    }

    // ZREMRANGEBYSCORE key min max
    // Remove all members in a sorted set within the given scores
    void Zremrangebyscore(std::ostream& out, const std::string& key, const std::string& min, const std::string& max)
    {
        GenCmd(out, "ZREMRANGEBYSCORE", key, min, max);
    }



    // ZREMRANGEBYSCORE key min max
    // Remove all members in a sorted set within the given scores
    void Zremrangebyscore(std::ostream& out, const std::string& key, const std::string& min, int max)
    {
        GenCmd(out, "ZREMRANGEBYSCORE", key, min, max);
    }



    // ZREMRANGEBYSCORE key min max
    // Remove all members in a sorted set within the given scores
    void Zremrangebyscore(std::ostream& out, const std::string& key, int min, const std::string& max)
    {
        GenCmd(out, "ZREMRANGEBYSCORE", key, min, max);
    }



    // ZREMRANGEBYSCORE key min max
    // Remove all members in a sorted set within the given scores
    void Zremrangebyscore(std::ostream& out, const std::string& key, int min, int max)
    {
        GenCmd(out, "ZREMRANGEBYSCORE", key, min, max);
    }

    // ZREVRANGE key start stop [WITHSCORES]
    // Return a range of members in a sorted set, by index, with scores ordered from high to low
    void Zrevrange(std::ostream& out, const std::string& key, int start, int stop /*[WITHSCORES]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREVRANGEBYSCORE key max min [WITHSCORES] [LIMIT offset count]
    // Return a range of members in a sorted set, by score, with scores ordered from high to low
    void Zrevrangebyscore(std::ostream& out, const std::string& key, const std::string& max, const std::string& min /*[WITHSCORES]*/ /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREVRANGEBYSCORE key max min [WITHSCORES] [LIMIT offset count]
    // Return a range of members in a sorted set, by score, with scores ordered from high to low
    void Zrevrangebyscore(std::ostream& out, const std::string& key, const std::string& max, int min /*[WITHSCORES]*/ /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREVRANGEBYSCORE key max min [WITHSCORES] [LIMIT offset count]
    // Return a range of members in a sorted set, by score, with scores ordered from high to low
    void Zrevrangebyscore(std::ostream& out, const std::string& key, int max, const std::string& min /*[WITHSCORES]*/ /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
    }

    // ZREVRANGEBYSCORE key max min [WITHSCORES] [LIMIT offset count]
    // Return a range of members in a sorted set, by score, with scores ordered from high to low
    void Zrevrangebyscore(std::ostream& out, const std::string& key, int max, int min /*[WITHSCORES]*/ /*[LIMIT offset count]*/)
    {
        static_assert(false,"not impl");
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
    void Zunionstore(std::ostream& out, const std::string& destination, size_t numkeys, const std::string& key /*[key ...]*/ /*[WEIGHTS weight [weight ...]]*/ /*[AGGREGATE SUM|MIN|MAX]*/)
    {
        static_assert(false,"not impl");
    }

    // ZSCAN key cursor [MATCH pattern] [COUNT count]
    // Incrementally iterate sorted sets elements and associated scores
    void Zscan(std::ostream& out, const std::string& key, int cursor /*[MATCH pattern]*/ /*[COUNT count]*/)
    {
        static_assert(false,"not impl");
    }

} // namespace SortedSetCommands

# endif // SORTEDSETCOMMANDS_H_
