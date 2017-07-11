
# ifndef SCRIPTINGCOMMANDS_H_
# define SCRIPTINGCOMMANDS_H_
# include "Util.h"
namespace ScriptingCommands
{

    // EVAL script numkeys key [key ...] arg [arg ...]
    // Execute a Lua script server side
    void Eval(std::ostream& out, const std::string& script, size_t numkeys, const std::string& key /*[key ...]*/, const std::string& arg /*[arg ...]*/)
    {
        static_assert(false,"not impl");
    }

    // EVAL script numkeys key [key ...] arg [arg ...]
    // Execute a Lua script server side
    void Eval(std::ostream& out, const std::string& script, size_t numkeys, const std::string& key /*[key ...]*/, const std::vector<char>& arg /*[arg ...]*/)
    {
        static_assert(false,"not impl");
    }

    // EVALSHA sha1 numkeys key [key ...] arg [arg ...]
    // Execute a Lua script server side
    void Evalsha(std::ostream& out, const std::string& sha1, size_t numkeys, const std::string& key /*[key ...]*/, const std::string& arg /*[arg ...]*/)
    {
        static_assert(false,"not impl");
    }

    // EVALSHA sha1 numkeys key [key ...] arg [arg ...]
    // Execute a Lua script server side
    void Evalsha(std::ostream& out, const std::string& sha1, size_t numkeys, const std::string& key /*[key ...]*/, const std::vector<char>& arg /*[arg ...]*/)
    {
        static_assert(false,"not impl");
    }

    // EVALSHA sha1 numkeys key [key ...] arg [arg ...]
    // Execute a Lua script server side
    void Evalsha(std::ostream& out, const std::vector<char>& sha1, size_t numkeys, const std::string& key /*[key ...]*/, const std::string& arg /*[arg ...]*/)
    {
        static_assert(false,"not impl");
    }

    // EVALSHA sha1 numkeys key [key ...] arg [arg ...]
    // Execute a Lua script server side
    void Evalsha(std::ostream& out, const std::vector<char>& sha1, size_t numkeys, const std::string& key /*[key ...]*/, const std::vector<char>& arg /*[arg ...]*/)
    {
        static_assert(false,"not impl");
    }

    // SCRIPT DEBUG YES|SYNC|NO
    // Set the debug mode for executed scripts.
    void ScriptDebug(std::ostream& out, ScriptDebugModes yesSyncNo)
    {
        GenCmd(out, "SCRIPT DEBUG", yesSyncNo);
    }

    // SCRIPT EXISTS sha1 [sha1 ...]
    // Check existence of scripts in the script cache.
    void ScriptExists(std::ostream& out, const std::string& sha1 /*[sha1 ...]*/)
    {
        static_assert(false,"not impl");
    }

    // SCRIPT EXISTS sha1 [sha1 ...]
    // Check existence of scripts in the script cache.
    void ScriptExists(std::ostream& out, const std::vector<char>& sha1 /*[sha1 ...]*/)
    {
        static_assert(false,"not impl");
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
    void ScriptLoad(std::ostream& out, const std::string& script)
    {
        GenCmd(out, "SCRIPT LOAD", script);
    }

} // namespace ScriptingCommands

# endif // SCRIPTINGCOMMANDS_H_
