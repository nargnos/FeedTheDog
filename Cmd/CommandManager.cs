using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel.Composition;
using System.ComponentModel.Composition.Hosting;
using System.Reflection;
using System.IO;
using System.Threading;

namespace CmdCore
{
    // 命令控制，后面会跟邮件部分（未写）配合
    // 这里只保留基本控制功能，其它用类库扩展，pi如果不支持mef可能要修改
    // TODO: update（热更新，需要下载邮件附件）\实时交互(估计邮件方式比较不好处理)模式\卸除插件\下载文件（邮件提交任务下载）
    // TODO: 添加对其它程序调用的功能（弄个shell，另一个模块）
    public static class CommandManager
    {
        private const string path_ = @".\";
        static CommandManager()
        {
            cmdLock_ = new Mutex();
            nativeArgCmd_ = new Dictionary<string, Func<ICommand, Result>>()
            {
                { "?", (cmd)=>new Result(true, cmd.Help) },
                { "ver", (cmd)=>new Result(true, cmd.Version.ToString()) },
                { "what", (cmd)=>new Result(true, cmd.Description) }
            };

            OnReload_ += UpdateCmdList;
            if (!Reload(path_))
            {
                Reload(string.Empty);
            }
        }

        private static void UpdateCmdList()
        {
            if (cmds_ == null)
            {
                allCmds_ = string.Empty;
                return;
            }
            var result = from info in cmds_ orderby info.Key select $"{info.Key,8}\t-\t{info.Value.Description}";
            allCmds_ = string.Join(Environment.NewLine, result);
        }

        public static bool Reload(string path)
        {
            var ac = new AggregateCatalog();
            ac.Catalogs.Add(new AssemblyCatalog(Assembly.GetExecutingAssembly()));
            if (!string.IsNullOrEmpty(path) && Directory.Exists(path))
            {
                ac.Catalogs.Add(new DirectoryCatalog(path));
            }
            var ctn = new CompositionContainer(ac);
            var exps = from item in ctn.GetExports<ICommand>()
                       let addon = item.Value
                       let addonName = addon.Name.Trim()
                       where !string.IsNullOrEmpty(addonName)
                       orderby addon.Version descending
                       group addon by addonName into grp
                       select new { Name = grp.Key, Addon = grp.First() };
            try
            {
                cmdLock_.WaitOne();
                cmds_ = exps.ToDictionary((item) => item.Name, (item) => item.Addon);
            }
            catch
            {
                return false;
            }
            finally
            {
                cmdLock_.ReleaseMutex();
            }
            OnReload_?.Invoke();
            return true;
        }
        public static string Cmds
        {
            get
            {
                return allCmds_;
            }
        }
        private static ICommand GetCommand(string name)
        {
            try
            {
                cmdLock_.WaitOne();
                return cmds_[name].Clone();
            }
            catch
            {
                return null;
            }
            finally
            {
                cmdLock_.ReleaseMutex();
            }
        }

        public static Result DoCommand(string line)
        {
            line = line.Trim();
            // 忽略掉空白命令
            if (string.IsNullOrEmpty(line))
            {
                return new Result(true, string.Empty);
            }
            var args = line.Split(' ');

            var cmd = GetCommand(args[0]);
            if (cmd == null)
            {
                return new Result(false, "命令不存在");
            }

            if (args.Length == 2 && nativeArgCmd_.ContainsKey(args[1]))
            {
                return nativeArgCmd_[args[1]](cmd);
            }

            try
            {
                return cmd.Do(args.Skip(1).ToArray());
            }
            catch (Exception e)
            {
                return new Result(false, e.Message);
            }
        }


        private static Dictionary<string, Func<ICommand, Result>> nativeArgCmd_;

        private static Mutex cmdLock_;
        private static string allCmds_;
        private static event Action OnReload_;
        private static Dictionary<string, ICommand> cmds_;
    }
}
