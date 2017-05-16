import Command
class Test(Command.ICommand):
    @staticmethod
    def Cmd():
        return "test"

    @staticmethod
    def Version():
        return 1

    @staticmethod
    def Description():
        return "hello world"

    @staticmethod
    def Help(self):
        return "test"

    def Run(self, mng):
        return Description()