from importlib import import_module
from importlib import reload
from imp import find_module
import os
import sys
import Config

pluginPath = Config.CONFIG["pluginPath"]
sys.path.append(pluginPath)

def LoadModuleClass(module, classType):
    ret = []

    for m in dir(module):
        try:
            attr = getattr(module, m)
            if issubclass(attr, classType):
                ret.append(attr)
        except:
            pass            
    return ret


def LoadPlugins(classType):
    dirs = (d for d in os.listdir(pluginPath) if not (
        d.startswith('__') or d.endswith('__')))
    ret = []
    for d in dirs:
        try:
            module = find_module(d)
            module = reload(module)
        except:
            module = import_module(d)
        ret+=LoadModuleClass(module, classType)
    return ret
