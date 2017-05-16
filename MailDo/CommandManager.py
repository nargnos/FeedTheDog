from Command import ICommand
from LoadPlugin import LoadPlugins


class ListCommands(ICommand):
    @staticmethod
    def Cmd():
        return "all"

    @staticmethod
    def Version():
        return 0

    @staticmethod
    def Description():
        return "显示所有命令"

    @staticmethod
    def Help(self):
        return "all"

    def Run(self, mng):
        return "\n".join(mng.ShowAll())


class DoPython(ICommand):
    @staticmethod
    def Cmd():
        return "exec"

    @staticmethod
    def Version():
        return 0

    @staticmethod
    def Description():
        return "执行py"

    @staticmethod
    def Help():
        return "exec code"

    def Run(self, mng, *code):
        return exec(' '.join(code), globals().copy())


class CmdDict(object):
    __commands = dict()

    def Add(self, cmdClass):
        if not issubclass(cmdClass, ICommand):
            return
        if cmdClass.Cmd() not in self.__commands or \
                cmdClass.Version() > self.__commands[cmdClass.Cmd()].Version():
            self.__commands[cmdClass.Cmd()] = cmdClass

    def ShowAll(self):
        return ["{0} - {1}".format(item.Cmd(), item.Description()) for item in self.__commands.values()]

    def Run(self, cmdName, *args):
        try:
            obj = self.__commands[cmdName]()
            return str(obj(self, *args))
        except KeyError as e:
            return "不存在命令: {}".format(cmdName)
        except Exception as e:
            return str(e)

    def Clear(self):
        self.__commands.clear()


class CommandManager(object):
    __cmd = CmdDict()

    def __init__(self):
        self.FlushCommands()

    def __AddDefaultCommand(self):
        self.__cmd.Add(ListCommands)

    def Run(self, cmdName, *args):
        return self.__cmd.Run(cmdName, *args)

    def FlushCommands(self):
        self.__cmd.Clear()
        self.__AddDefaultCommand()
        cmd = LoadPlugins(ICommand)
        if len(cmd) > 0:
            for item in cmd:
                self.__cmd.Add(item)


