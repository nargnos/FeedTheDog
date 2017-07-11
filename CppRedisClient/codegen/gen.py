#!/usr/bin/env python3
# 简化一些手工操作，变参的基本要手工，还有就是中间有空格的命令是要分开的，这里没分，也不多，手工改改就行
import os
import itertools
import string
import GetDoc
import FuncCodeBuilder

fileTemplate = string.Template('''
# ifndef ${Guard}_H_
# define ${Guard}_H_
# include "Util.h"
namespace ${Group}
{
${Func}
} // namespace ${Group}

# endif // ${Guard}_H_
''')

import Data
cmds =Data.GetData() #GetDoc.GetCommands()
print("共 {} 组".format(len(cmds)))
print("共 {} 个命令".format(sum([len(v) for v in cmds.values()])))

path = ".\\gen"
path = os.path.abspath(path)
print("output: {}".format(path))
if not os.path.exists(path):
    os.mkdir(path)

fcb=FuncCodeBuilder.FuncCodeBuilder()

for grpName,info in cmds.items():
    headerPath = os.path.join(path, "{}Commands.h".format(grpName))
    nsName = "{}Commands".format(grpName)
    # 此时缺Func
    fileTemplateDict = {"Group": nsName, "Guard": nsName.upper()}
    fileTemplateDict["Func"]=""
    for item in info:
        ret=fcb.Build(item)
        if ret!=None:
            fileTemplateDict["Func"]+=ret
        
    code=fileTemplate.substitute(fileTemplateDict)
    # print(code)
    with open(headerPath, mode='w', encoding="utf8") as file:
        file.write(code)
  