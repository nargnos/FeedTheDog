using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CmdCore;
using System.Diagnostics;

namespace MailShell
{
    class Program
    {
        static void Main(string[] args)
        {
            // 测试
            var mng = new CommandManager();
            var res = mng.DoCommand("?");
            if (res.IsSucceed)
            {
                Console.WriteLine(res.Message);
                Debug.WriteLine(res.Message);
            }
            else
            {
                Console.WriteLine("Faild");
                Debug.WriteLine("Faild");
            }
        }
    }
}
