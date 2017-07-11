import re
import collections

class ArgAnalysis(object):

    def __init__(self):
        self.__conv = {}
        # 对应参数可表示类型

        self.__RegType("String", ["const std::string& {Arg}"], set([
                       "key", "ip", "host", "password", "parameter", "connection-name", "destination", "source", "script", "field", "member", "pattern", "channel", "member1", "member2", "max", "min", "score", "bit", "destkey", "sourcekey", "command-name", "subcommand", 'key|""']))

        self.__RegType("Int", ["int {Arg}"], set(["offset", "numslaves", "index", "index2", "newkey", "stop", "start", "max", "min",
                                                  "increment", "decrement", "count", "end", "timeout", "config-epoch", "slot", "ttl", "score", "destination-db", "cursor"]))

        self.__RegType("UInt", ["size_t {Arg}"], set(["numkeys", "db"]))

        self.__RegType("Float", ["float {Arg}"], set([
                       "incrementFloat", "decrementFloat", "latitude", "longitude", "radius"]))

        self.__RegType("Short", ["unsigned short {Arg}"], set(["port"]))

        self.__RegType("ValueChar", ["const std::string& {Arg}", "const std::vector<char>& {Arg}"], set([
                       "value", "message", "node-id", "pivot", "element", "serialized-value", "arg", "sha1"]))

        self.__RegType("Time", ["time_t {Arg}"], set([
                       "timestamp", "milliseconds-timestamp"]))

        self.__RegType(
            "Second", ["std::chrono::seconds {Arg}"], set(["seconds"]))

        self.__RegType("Milliseconds", [
                       "std::chrono::milliseconds {Arg}"], set(["milliseconds"]))

        self.__RegType("InsertPos", ["InsertPos {Arg}"], set(["BEFORE|AFTER"]))

        self.__RegType(
            "ReplyModes", ["ReplyModes {Arg}"], set(["ON|OFF|SKIP"]))

        self.__RegType("ScriptDebugModes", [
                       "ScriptDebugModes {Arg}"], set(["YES|SYNC|NO"]))

        self.__RegType("Operation", [
                       "Operation {Arg}"], set(["operation"]))

        self.__RegType("Distance", [
                       "Distance {Arg}"], set(["m|km|ft|mi"]))
        self.__RegType("SlotModes", [
                       "SlotModes {Arg}"], set(["IMPORTING|MIGRATING|STABLE|NODE"]))

    def __RegType(self, t, define, keys):
        if t in self.__conv:
            raise Exception("type dup")
        self.__conv[t] = {"Keys": keys, "Def": define}

    def GetType(self, key):
        if not isinstance(key, str):
            raise
        ret = {}
        for item in self.__conv.values():
            if key in item["Keys"]:
                if "Def" in ret:
                    ret["Def"] += item["Def"]
                else:
                    ret["Def"] = item["Def"].copy()

        if "[" in key:
            ret["Type"] = "Dym"
        elif len(ret) == 0:
            ret["Type"] = "Unknown"
        else:
            ret["Type"] = "None"
        return ret

    def Analysis(self, args):
        if not isinstance(args, str):
            raise Exception("str type error")
        if len(set("[].") & set(args)) == 0:
            # 定参
            return self.__Do(args.split())
        # 处理变参
        return self.__Do(re.findall('[-|\w"]+|\[.+?\]\]?', args))

    def __ConvName(self, name):
        s = re.split('[-|]', name)
        if len(s) == 1:
            return name
        return s[0].lower() + "".join([item.capitalize() for item in s[1:]])

    def __Do(self, args):
        nameCount = collections.Counter(args)
        nameCount = {k: 0 for k, v in nameCount.items() if v > 1}

        ret = []
        for item in args:
            tmp = self.GetType(item)
            if tmp["Type"]=="Dym":
                tmp["Def"]=[item]
            else:
                if item in nameCount:
                    nameCount[item] += 1
                    #tmp["Name"] = "{}{}".format(item, nameCount[item])
                    tmp["ArgName"] = "{}{}".format(self.__ConvName(item), nameCount[item])
                else:
                    #tmp["Name"] = item
                    tmp["ArgName"] = self.__ConvName(item)
            ret.append(tmp)
        return ret


if __name__ == "__main__":
    import Data
    args = Data.GetArgs()
    aa = ArgAnalysis()
    undoneArg = set()
    # print("固定参数")
    # fixedArg = [item for item in args if len(set("[].") & set(item)) == 0]

    # for item in fixedArg:
    #     ret = aa.Analysis(item)
    #     undoneArg = undoneArg.union([arg["Name"]
    #                                  for arg in ret if arg["Type"] == "Unknown"])
    #     #print(ret, end='\r\n')

    print("变参")
    dymArg = [item for item in args if len(set("[].") & set(item)) > 0]
    # print("\r\n".join(dymArg))
    for item in dymArg:
        ret = aa.Analysis(item)
        undoneArg = undoneArg.union([arg["Name"]
                                     for arg in ret if arg["Type"] == "Dym"])
        #print(ret, end="\r\n")
    print("\r\n".join(undoneArg))
