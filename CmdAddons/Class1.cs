using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CmdCore;
using System.ComponentModel.Composition;

namespace CmdAddons
{
    public class Class1 : ICommand
    {
        public string Description { get; } = "测试";

        public string Help { get; } = "测试";

        public string Name { get; } = "test";

        public int Version { get; } = 0;

        public ICommand Clone()
        {
            return new Class1($"hello world {DateTime.Now}");
        }
        [ImportingConstructor]
        public Class1()
        {
            msg_ = string.Empty;
        }

        public Class1(string msg)
        {
            msg_ = msg;
        }
        public Result Do(CommandManager mng, params string[] args)
        {
            return new Result(true, msg_);
        }
        private string msg_;
    }
}
