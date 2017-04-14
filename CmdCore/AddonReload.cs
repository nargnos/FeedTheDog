using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CmdCore
{
    class AddonReload : ICommand
    {
        public int Version { get; } = int.MaxValue;

        public string Description { get; } = "重新载入插件";
        public string Help { get; } = "reload [path]";
        public string Name { get; } = "reload";

        public ICommand Clone()
        {
            return this;
        }
        public Result Do(CommandManager mng, params string[] args)
        {
            bool res = true;
            if (args.Length == 0)
            {
                res = mng.Reload(string.Empty);
            }
            else
            {
                res = mng.Reload(string.Join(" ", args));
            }
            return new Result(res, res ? string.Empty : "载入失败");
        }
    }
}
