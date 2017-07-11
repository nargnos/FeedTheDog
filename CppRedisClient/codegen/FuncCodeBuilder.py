import itertools
from string import Template
import ArgAnalysis

class FuncCodeBuilder(object):
    __analysis = ArgAnalysis.ArgAnalysis()
    __funcTemplate = Template('''
    // ${Cmd} ${Args}
    // ${Summary}
    void ${Name}(std::ostream& out${Param})
    {
        ${Code}
    }
''')
    __codeTemplate = 'GenCmd(out, "{Cmd}"{Vals});'

    def Build(self, infoDict):
        if not isinstance(infoDict, dict):
            raise Exception("arg error")
        info = infoDict.copy()
        if "Args" not in infoDict or len(infoDict["Args"]) == 0:
            return self.__BuildNoneArgsCode(info)

        args = self.__analysis.Analysis(infoDict["Args"])
        if any([item["Type"] == "Unknown" for item in args]):
            raise Exception("有未处理的参数: {}".format(args))
        if any([item["Type"] == "Dym" for item in args]):
            # 处理变参
            return self.__BuildDymArgCode(info, args)
        return self.__BuildFixedArgsCode(info, args)

    def __BuildNoneArgsCode(self, info):
        info["Param"] = ""
        info["Code"] = self.__codeTemplate.format(Cmd=info["Cmd"], Vals="")
        return self.__funcTemplate.substitute(info)

    def __BuildParamCode(self, paramlist, args, cmdName):
        ret = []
        for params in paramlist:
            tmp = {}
            names = [item["ArgName"] for item in args]
            tmp["Param"] = ", "
            tmp["Param"] += ", ".join([p.format(Arg=name)
                                       for p, name in zip(params, names)])
            codeArg = ", " + ", ".join(names)
            tmp["Code"] = self.__codeTemplate.format(Cmd=cmdName, Vals=codeArg)
            ret.append(tmp)
        return ret

    def __BuildFixedArgsCode(self, info, args):
        pl = self.__GetParamList(args)

        code = self.__BuildParamCode(pl, args, info["Cmd"])
        for item in code:
            item.update(info)
        ret = "\r\n".join([self.__funcTemplate.substitute(item)
                           for item in code])
        return ret

    def __GetParamList(self, args):
        tmpList = [item["Def"] for item in args]
        return [item for item in itertools.product(*tmpList)]
    
    def __BuildDymArgCode(self, info, args):        
        pl = self.__GetParamList(args)
        ret=""
        for p in pl:
            param=""
            for d,arg in zip(p,args):
                if "ArgName" in arg:
                    param+=", "+d.format(Arg=arg["ArgName"])
                else:
                    param+=" /*{}*/".format(d)
            # 没有什么共同特征，只能手动来
            tmpDict={"Param":param,"Code":'static_assert(false,"not impl");'}
            tmpDict.update(info)
            ret+=self.__funcTemplate.substitute(tmpDict)
        #print(xx)
        return ret
    

if __name__ == "__main__":
    import Data
    data = Data.GetData()
    b = FuncCodeBuilder()
    ret = set([b.Build(item) for grp, nodes in data.items() for item in nodes])
    # ret.remove(None)
    print("\r\n".join(ret))
