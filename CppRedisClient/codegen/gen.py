#!/usr/bin/env python3
from string import Template
import os
from urllib import request
from urllib import response
import gzip
import lxml.etree as ET
import re


def GetHtml(url):
    header = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0',
              'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
              'Accept-Encoding': 'gzip, deflate, br',
              'Connection': 'keep-alive',
              'Upgrade-Insecure-Requests': 1}
    req = request.Request(url, headers=header)

    with request.urlopen(req) as r:
        return gzip.decompress(r.read())
    return None


def GetCommands(url="https://redis.io/commands"):
    xpath = '//*[@id="commands"]/div/ul/li[@data-name]'
    search = {"Group": '@data-group',
              #"Name": '@data-name',
              #"Link": 'a/@href',
              "Cmd": 'a/span[@class="command"]/text()',
              "Args": 'a/span[@class="command"]/span[@class="args"]/text()',
              "Summary": 'a/span[@class="summary"]/text()'
              }
    page = GetHtml(url)
    htm = ET.HTML(page)

    nodes = htm.xpath(xpath)

    ret = {}
    for i in nodes:
        d = {}
        for name, xp in search.items():
            select = " ".join("".join(i.xpath(xp)).split())
            d[name] = select.strip()
        name = "".join([x.capitalize()
                        for x in re.split("\\s+|-|[\\r\\n]", d["Cmd"])])
        d["Name"] = name

        group = "".join([x.capitalize()
                         for x in re.split("_|\\s+|-|[\\r\\n]", d["Group"])])
        d["Group"] = group
        if group in ret:
            ret[group].append(d)
        else:
            ret[group] = [d]
    return ret


cmds = GetCommands()


fileTemplate = Template('''
#ifndef ${Guard}_H_
#define ${Guard}_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace ${Group}
{
${Func}
} // namespace ${Group}

#endif // ${Guard}_H_
''')

funcTemplate = Template('''
	// ${Cmd} ${Args}
	// ${Summary}
	void ${Name}(std::ostream& out${Param})
	{
		${Code}
	}
''')

path = ".\\gen"
path = os.path.abspath(path)
print("output: {}".format(path))
if not os.path.exists(path):
    os.mkdir(path)

param = {
    "key":
    [{
        "Param": ", const std::string& key",
        "Code": 'GenCmd(out, "{}", key);'
    }],
    "key value":
    [
        {
            "Param": ", const std::string& key, const std::vector<char>& val",
            "Code": 'GenCmd(out, "{}", key, val);'
        },
        {
            "Param": ", const std::string& key, const std::string& val",
            "Code": 'GenCmd(out, "{}", key, val);'
        }
    ],
    "index":
    [{
        "Param": ", int index",
        "Code": 'GenCmd(out, "{}", index);'
    }],
    "node-id":
    [{
        "Param": ", const std::string& id",
        "Code": 'GenCmd(out, "{}", id);'
    }],
    "index index":
    [{
        "Param": ", int index1, int index2",
        "Code": 'GenCmd(out, "{}", index1, index2);'
    }],
    "numslaves timeout":
    [{
        "Param": ", int numslaves, int timeout",
        "Code": 'GenCmd(out, "{}", numslaves, timeout);'
    }],
    "key newkey":
    [{
        "Param": ", const std::string& key, const std::string& newkey",
        "Code": 'GenCmd(out, "{}", key, newkey);'
    }],
    "key seconds":
    [{
        "Param": ", const std::string& key, int sec",
        "Code": 'GenCmd(out, "{}", key, sec);'
    }],
    "key db":
    [{
        "Param": ", const std::string& key, int db",
        "Code": 'GenCmd(out, "{}", key, db);'
    }],
    "key milliseconds":
    [{
        "Param": ", const std::string& key, int ms",
        "Code": 'GenCmd(out, "{}", key, ms);'
    }],
    "message":
    [{
        "Param": ", const std::string& msg",
        "Code": 'GenCmd(out, "{}", msg);'
    }],
    "password":
    [{
        "Param": ", const std::string& passwd",
        "Code": 'GenCmd(out, "{}", passwd);'
    }],
    "pattern":
    [{
        "Param": ", const std::string& pattern",
        "Code": 'GenCmd(out, "{}", pattern);'
    }],
    "key timestamp":
    [{
        "Param": ", const std::string& key, time_t t",
        "Code": 'GenCmd(out, "{}", key, t);'
    }],
    "key milliseconds-timestamp":
    [{
        "Param": ", const std::string& key, time_t t",
        "Code": 'GenCmd(out, "{}", key, t);'
    }],
    "key decrement":
    [{
        "Param": ", const std::string& key, int d",
        "Code": 'GenCmd(out, "{}", key, d);'
    }],
    "key offset":
    [{
        "Param": ", const std::string& key, int offset",
        "Code": 'GenCmd(out, "{}", key, offset);'
    }],
    "key start end":
    [{
        "Param": ", const std::string& key, int start, int end",
        "Code": 'GenCmd(out, "{}", key, start, end);'
    }],
    "key increment":
    [{
        "Param": ", const std::string& key, int i",
        "Code": 'GenCmd(out, "{}", key, i);'
    }],
    "key milliseconds value":
    [
        {
            "Param": ", const std::string& key, int ms, const std::vector<char>& val",
            "Code": 'GenCmd(out, "{}", key, ms, val);'
        },
        {
            "Param": ", const std::string& key, int ms, const std::string& val",
            "Code": 'GenCmd(out, "{}", key, ms, val);'
        }
    ],
    "key offset value":
    [
        {
            "Param": ", const std::string& key, int offset, const std::vector<char>& val",
            "Code": 'GenCmd(out, "{}", offset, offset, val);'
        },
        {
            "Param": ", const std::string& key, int offset, const std::string& val",
            "Code": 'GenCmd(out, "{}", offset, offset, val);'
        }
    ],
    "key seconds value":
    [
        {
            "Param": ", const std::string& key, int s, const std::vector<char>& val",
            "Code": 'GenCmd(out, "{}", s, val);'
        },
        {
            "Param": ", const std::string& key, int s, const std::string& val",
            "Code": 'GenCmd(out, "{}", s, val);'
        }
    ],
    "key min max":
    [{
        "Param": ", const std::string& key, int min, int max",
        "Code": 'GenCmd(out, "{}", key, min, max);'
    }],
    "timeout":
    [{
        "Param": ", int timeout",
        "Code": 'GenCmd(out, "{}", timeout);'
    }],
    "key index":
    [{
        "Param": ", const std::string& key, int index",
        "Code": 'GenCmd(out, "{}", key, index);'
    }],
    "key field":
    [{
        "Param": ", const std::string& key, const std::string& field",
        "Code": 'GenCmd(out, "{}", key, field);'
    }],
    "key member":
    [{
        "Param": ", const std::string& key, const std::string& member",
        "Code": 'GenCmd(out, "{}", key, member);'
    }],
    "key field value":
    [
        {
            "Param": ", const std::string& key, const std::string& field, const std::vector<char>& val",
            "Code": 'GenCmd(out, "{}", key, field, val);'
        },
        {
            "Param": ", const std::string& key, const std::string& field, const std::string& val",
            "Code": 'GenCmd(out, "{}", key, field, val);'
        }
    ],
    "key incrementFloat":
    [{
        "Param": ", const std::string& key, float i",
        "Code": 'GenCmd(out, "{}", key, i);'
    }]
}
# 微调参数表示
# 某些类型不同的用相同的参数标识，需要区分
cmdArgs={"INCRBYFLOAT":"key incrementFloat"}


def Gen(arr):
    ret = []
    left = 0
    done = 0
    leftArg = []
    for grpName, item in arr.items():
        headerPath = os.path.join(path, "{}Commands.h".format(grpName))
        nsName = "{}Commands".format(grpName)
        tmpFile = {"Group": nsName, "Guard": nsName.upper()}

        func = ""
        for i in item:
            if i["Cmd"] in cmdArgs:
                i["Args"]=cmdArgs[i["Cmd"]]
            if len(i["Args"]) == 0:
                i["Code"] = 'GenCmd(out, "{}");'.format(i["Cmd"])
                i["Param"] = ""
                done += 1
                func += funcTemplate.substitute(i)
            else:
                if i["Args"] in param:
                    ps = param[i["Args"]]
                    for p in ps:
                        try:
                            i["Code"] = p["Code"].format(i["Cmd"])
                            i["Param"] = p["Param"]
                            func += funcTemplate.substitute(i)
                        except Exception as e:
                            print(e)
                            raise
                    done += 1
                else:
                    i["Code"] = "// TODO: {}\r\n\t\tassert(false);".format(
                        i["Cmd"])
                    i["Param"] = ""
                    left += 1
                    func += funcTemplate.substitute(i)
                    leftArg.append(i["Args"])
        tmpFile["Func"] = func
        res = fileTemplate.substitute(tmpFile)

        with open(headerPath, mode='w', encoding="utf8") as file:
            file.write(res)
            ret.append(headerPath)
    return ret, left, done, leftArg


paths, left, done, leftArg = Gen(cmds)
print("count:", len(paths))
print("left:", left)
print("done:", done)
print("left arg:", "\r\n".join(set(leftArg)))

