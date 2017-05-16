import abc


class ICommand(metaclass=abc.ABCMeta):
    @abc.abstractstaticmethod
    def Cmd(self):
        raise NotImplementedError()

    @abc.abstractstaticmethod
    def Version(self):
        raise NotImplementedError()

    @abc.abstractstaticmethod
    def Description(self):
        raise NotImplementedError()

    @abc.abstractstaticmethod
    def Help(self):
        raise NotImplementedError()

    @abc.abstractclassmethod
    def Run(self, mng, *arg):
        raise NotImplementedError()

    def __call__(self, mng, *args):
        if len(args) == 1 and (args[0] == "help" or args[0] == "?"):
            return self.Help()
        else:
            return self.Run(mng, *args)
            
