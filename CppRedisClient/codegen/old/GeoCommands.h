
# ifndef GEOCOMMANDS_H_
# define GEOCOMMANDS_H_
# include "Util.h"
namespace GeoCommands
{

    // GEOADD key longitude latitude member [longitude latitude member ...]
    // Add one or more geospatial items in the geospatial index represented using a sorted set
    void Geoadd(std::ostream& out, const std::string& key, float longitude, float latitude, const std::string& member /*[longitude latitude member ...]*/)
    {
        static_assert(false,"not impl");
    }

    // GEOHASH key member [member ...]
    // Returns members of a geospatial index as standard geohash strings
    void Geohash(std::ostream& out, const std::string& key, const std::string& member /*[member ...]*/)
    {
        static_assert(false,"not impl");
    }

    // GEOPOS key member [member ...]
    // Returns longitude and latitude of members of a geospatial index
    void Geopos(std::ostream& out, const std::string& key, const std::string& member /*[member ...]*/)
    {
        static_assert(false,"not impl");
    }

    // GEODIST key member1 member2 [unit]
    // Returns the distance between two members of a geospatial index
    void Geodist(std::ostream& out, const std::string& key, const std::string& member1, const std::string& member2 /*[unit]*/)
    {
        static_assert(false,"not impl");
    }

    // GEORADIUS key longitude latitude radius m|km|ft|mi [WITHCOORD] [WITHDIST] [WITHHASH] [COUNT count] [ASC|DESC] [STORE key] [STOREDIST key]
    // Query a sorted set representing a geospatial index to fetch members matching a given maximum distance from a point
    void Georadius(std::ostream& out, const std::string& key, float longitude, float latitude, float radius, Distance mKmFtMi /*[WITHCOORD]*/ /*[WITHDIST]*/ /*[WITHHASH]*/ /*[COUNT count]*/ /*[ASC|DESC]*/ /*[STORE key]*/ /*[STOREDIST key]*/)
    {
        static_assert(false,"not impl");
    }

    // GEORADIUSBYMEMBER key member radius m|km|ft|mi [WITHCOORD] [WITHDIST] [WITHHASH] [COUNT count] [ASC|DESC] [STORE key] [STOREDIST key]
    // Query a sorted set representing a geospatial index to fetch members matching a given maximum distance from a member
    void Georadiusbymember(std::ostream& out, const std::string& key, const std::string& member, float radius, Distance mKmFtMi /*[WITHCOORD]*/ /*[WITHDIST]*/ /*[WITHHASH]*/ /*[COUNT count]*/ /*[ASC|DESC]*/ /*[STORE key]*/ /*[STOREDIST key]*/)
    {
        static_assert(false,"not impl");
    }

} // namespace GeoCommands

# endif // GEOCOMMANDS_H_
