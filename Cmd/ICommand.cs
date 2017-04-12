using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CmdCore
{
    [InheritedExport(typeof(ICommand))]
    public interface ICommand
    {
        // 需要存储自身状态的就new，没有就返回自身
        ICommand Clone();
        int Version { get; }
        string Name { get; }
        string Description { get; }
        string Help { get; }
        // 是否成功, 错误或结果字符串
        Result Do(params string[] args);
    }
}
