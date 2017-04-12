using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CmdCore
{
    public struct Result
    {
        public bool IsSucceed { get; private set; }
        public string Message { get; private set; }
        public Result(bool isSucceed, string msg)
        {
            IsSucceed = isSucceed;
            Message = msg;
        }
    }
}
