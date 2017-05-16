from CommandManager import CommandManager

mng = CommandManager()
mng.FlushCommands()

print(mng.Run("all"))
print(mng.Run("exec print(' hi');'i'"))
