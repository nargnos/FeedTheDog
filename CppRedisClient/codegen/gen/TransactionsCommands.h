
#ifndef TRANSACTIONSCOMMANDS_H_
#define TRANSACTIONSCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace TransactionsCommands
{

	// DISCARD 
	// Discard all commands issued after MULTI
	void Discard(std::ostream& out)
	{
		GenCmd(out, "DISCARD");
	}

	// EXEC 
	// Execute all commands issued after MULTI
	void Exec(std::ostream& out)
	{
		GenCmd(out, "EXEC");
	}

	// MULTI 
	// Mark the start of a transaction block
	void Multi(std::ostream& out)
	{
		GenCmd(out, "MULTI");
	}

	// UNWATCH 
	// Forget about all watched keys
	void Unwatch(std::ostream& out)
	{
		GenCmd(out, "UNWATCH");
	}

	// WATCH key [key ...]
	// Watch the given keys to determine execution of the MULTI/EXEC block
	void Watch(std::ostream& out)
	{
		// TODO: WATCH
		assert(false);
	}

} // namespace TransactionsCommands

#endif // TRANSACTIONSCOMMANDS_H_
