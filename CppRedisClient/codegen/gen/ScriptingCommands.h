
#ifndef SCRIPTINGCOMMANDS_H_
#define SCRIPTINGCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace ScriptingCommands
{

	// EVAL script numkeys key [key ...] arg [arg ...]
	// Execute a Lua script server side
	void Eval(std::ostream& out)
	{
		// TODO: EVAL
		assert(false);
	}

	// EVALSHA sha1 numkeys key [key ...] arg [arg ...]
	// Execute a Lua script server side
	void Evalsha(std::ostream& out)
	{
		// TODO: EVALSHA
		assert(false);
	}

	// SCRIPT DEBUG YES|SYNC|NO
	// Set the debug mode for executed scripts.
	void ScriptDebug(std::ostream& out)
	{
		// TODO: SCRIPT DEBUG
		assert(false);
	}

	// SCRIPT EXISTS sha1 [sha1 ...]
	// Check existence of scripts in the script cache.
	void ScriptExists(std::ostream& out)
	{
		// TODO: SCRIPT EXISTS
		assert(false);
	}

	// SCRIPT FLUSH 
	// Remove all the scripts from the script cache.
	void ScriptFlush(std::ostream& out)
	{
		GenCmd(out, "SCRIPT FLUSH");
	}

	// SCRIPT KILL 
	// Kill the script currently in execution.
	void ScriptKill(std::ostream& out)
	{
		GenCmd(out, "SCRIPT KILL");
	}

	// SCRIPT LOAD script
	// Load the specified Lua script into the script cache.
	void ScriptLoad(std::ostream& out)
	{
		// TODO: SCRIPT LOAD
		assert(false);
	}

} // namespace ScriptingCommands

#endif // SCRIPTINGCOMMANDS_H_
