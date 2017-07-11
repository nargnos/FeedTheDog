from urllib import request
from urllib import response
import gzip
import lxml.etree as ET
import re
defaultHeader = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0',
                 'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
                 'Accept-Encoding': 'gzip, deflate, br',
                 'Connection': 'keep-alive',
                 'Upgrade-Insecure-Requests': 1}


def GetHtml(url, header=defaultHeader):
    req = request.Request(url, headers=header)
    try:
        data = request.urlopen(req)
        return gzip.decompress(data.read())
    except Exception as e:
        print(e)
        raise


def GetCommands(url="https://redis.io/commands"):
    # 以Group分组，内容为每条命令的声明数组
    xpath = '//*[@id="commands"]/div/ul/li[@data-name]'
    search = {"Group": '@data-group',
              #"Name": '@data-name',
              #"Link": 'a/@href',
              "Cmd": 'a/span[@class="command"]/text()',
              "Args": 'a/span[@class="command"]/span[@class="args"]/text()',
              "Summary": 'a/span[@class="summary"]/text()'
              }
    page = GetHtml(url)
        
    pageEt = ET.HTML(page)

    nodes = pageEt.xpath(xpath)

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


if __name__ == "__main__":
    cmds = GetCommands()
    print(cmds)

    # args = set([item["Args"] for grp in cmds.values()
    #             for item in grp if len(item["Args"]) > 0])
    # print("\r\n".join(args))
