using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using CmdCore;
using System.Diagnostics;

namespace CmdTest
{
    [TestClass]
    public class CmdCoreTest
    {
        [TestMethod]
        public void TestHelp()
        {
            var mng = new CommandManager();
            var res = mng.DoCommand("?");
            Assert.IsTrue(res.IsSucceed);
            Debug.WriteLine(res.Message);
            res = mng.DoCommand("? ?");
            Assert.IsTrue(res.IsSucceed);
            res = mng.DoCommand("? ver");
            Assert.IsTrue(res.IsSucceed);
            Assert.AreEqual(res.Message, int.MaxValue.ToString());
            res = mng.DoCommand("? what");
            Assert.IsTrue(res.IsSucceed);
            res = mng.DoCommand(string.Empty);
            Assert.IsTrue(res.IsSucceed);
            res = mng.DoCommand("??");
            Assert.IsFalse(res.IsSucceed);


        }
        [TestMethod]
        public void TestReload()
        {
            var mng = new CommandManager();
            var res = mng.DoCommand("reload");
            Assert.IsTrue(res.IsSucceed);
            res = mng.DoCommand("reload ./");
            Assert.IsTrue(res.IsSucceed);
            res = mng.DoCommand("reload ?");
            Assert.IsTrue(res.IsSucceed);
            res = mng.DoCommand("reload ver");
            Assert.IsTrue(res.IsSucceed);
        }
        [TestMethod]
        public void TestLoadAddonFile()
        {
            var mng = new CommandManager();
            var res = mng.DoCommand("test");
            Assert.IsTrue(res.IsSucceed);
        }
        [TestMethod]
        public void TestDomain()
        {
            var dm = AppDomain.CreateDomain(Guid.NewGuid().ToString());
            var cur = Environment.CurrentDirectory;
            // 需要先编译出文件
            dm.ExecuteAssembly(@"..\..\..\MailShell\bin\Debug\MailShell.exe");
            AppDomain.Unload(dm);

        }
    }
}
