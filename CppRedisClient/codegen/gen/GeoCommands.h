
#ifndef GEOCOMMANDS_H_
#define GEOCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace GeoCommands
{

	// GEOADD key longitude latitude member [longitude latitude member ...]
	// Add one or more geospatial items in the geospatial index represented using a sorted set
	void Geoadd(std::ostream& out)
	{
		// TODO: GEOADD
		assert(false);
	}

	// GEOHASH key member [member ...]
	// Returns members of a geospatial index as standard geohash strings
	void Geohash(std::ostream& out)
	{
		// TODO: GEOHASH
		assert(false);
	}

	// GEOPOS key member [member ...]
	// Returns longitude and latitude of members of a geospatial index
	void Geopos(std::ostream& out)
	{
		// TODO: GEOPOS
		assert(false);
	}

	// GEODIST key member1 member2 [unit]
	// Returns the distance between two members of a geospatial index
	void Geodist(std::ostream& out)
	{
		// TODO: GEODIST
		assert(false);
	}

	// GEORADIUS key longitude latitude radius m|km|ft|mi [WITHCOORD] [WITHDIST] [WITHHASH] [COUNT count] [ASC|DESC] [STORE key] [STOREDIST key]
	// Query a sorted set representing a geospatial index to fetch members matching a given maximum distance from a point
	void Georadius(std::ostream& out)
	{
		// TODO: GEORADIUS
		assert(false);
	}

	// GEORADIUSBYMEMBER key member radius m|km|ft|mi [WITHCOORD] [WITHDIST] [WITHHASH] [COUNT count] [ASC|DESC] [STORE key] [STOREDIST key]
	// Query a sorted set representing a geospatial index to fetch members matching a given maximum distance from a member
	void Georadiusbymember(std::ostream& out)
	{
		// TODO: GEORADIUSBYMEMBER
		assert(false);
	}

} // namespace GeoCommands

#endif // GEOCOMMANDS_H_
