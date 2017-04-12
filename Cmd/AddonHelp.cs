using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CmdCore
{
    class AddonHelp : ICommand
    {
        public string Description { get; } = "列所有命令";
        public string Help { get; } = string.Empty;
        public string Name { get; } = "?";

        public int Version { get; } = int.MaxValue;

        public ICommand Clone()
        {
            return this;
        }
        public Result Do(params string[] args)
        {
            return new Result(true, CommandManager.Cmds);
        }
    }
}
