
#ifndef HYPERLOGLOGCOMMANDS_H_
#define HYPERLOGLOGCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace HyperloglogCommands
{

	// PFADD key element [element ...]
	// Adds the specified elements to the specified HyperLogLog.
	void Pfadd(std::ostream& out)
	{
		// TODO: PFADD
		assert(false);
	}

	// PFCOUNT key [key ...]
	// Return the approximated cardinality of the set(s) observed by the HyperLogLog at key(s).
	void Pfcount(std::ostream& out)
	{
		// TODO: PFCOUNT
		assert(false);
	}

	// PFMERGE destkey sourcekey [sourcekey ...]
	// Merge N different HyperLogLogs into a single one.
	void Pfmerge(std::ostream& out)
	{
		// TODO: PFMERGE
		assert(false);
	}

} // namespace HyperloglogCommands

#endif // HYPERLOGLOGCOMMANDS_H_
