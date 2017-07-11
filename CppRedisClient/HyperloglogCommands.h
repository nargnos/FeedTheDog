
# ifndef HYPERLOGLOGCOMMANDS_H_
# define HYPERLOGLOGCOMMANDS_H_
# include "Util.h"
namespace HyperloglogCommands
{

	// PFADD key element [element ...]
	// Adds the specified elements to the specified HyperLogLog.
	void Pfadd(std::ostream& out, const std::string& key, const std::string& element,
		const std::vector<std::string>& elements = {})
	{
		GenCmdWithSize(out, 3 + elements.size(), "PFADD", key, element, elements);
	}

	// PFADD key element [element ...]
	// Adds the specified elements to the specified HyperLogLog.
	void Pfadd(std::ostream& out, const std::string& key, const std::vector<char>& element,
		const std::vector<std::vector<char>>& elements = {})
	{
		GenCmdWithSize(out, 3 + elements.size(), "PFADD", key, element, elements);
	}

	// PFCOUNT key [key ...]
	// Return the approximated cardinality of the set(s) observed by the HyperLogLog at key(s).
	void Pfcount(std::ostream& out, const std::string& key,
		const std::vector<std::string>& keys = {})
	{
		GenCmdWithSize(out, 2 + keys.size(), "PFCOUNT", key, keys);
	}

	// PFMERGE destkey sourcekey [sourcekey ...]
	// Merge N different HyperLogLogs into a single one.
	void Pfmerge(std::ostream& out, const std::string& destkey, const std::string& sourcekey,
		const std::vector<std::string>& sourcekeys = {})
	{
		GenCmdWithSize(out, 3 + sourcekeys.size(), "PFMERGE", destkey, sourcekey, sourcekeys);
	}

} // namespace HyperloglogCommands

# endif // HYPERLOGLOGCOMMANDS_H_
