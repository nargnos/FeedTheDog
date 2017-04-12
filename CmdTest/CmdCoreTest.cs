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
            var res = CommandManager.DoCommand("?");
            Assert.IsTrue(res.IsSucceed);
            Debug.WriteLine(res.Message);
            res = CommandManager.DoCommand("? ?");
            Assert.IsTrue(res.IsSucceed);
            res = CommandManager.DoCommand("? ver");
            Assert.IsTrue(res.IsSucceed);
            Assert.AreEqual(res.Message, int.MaxValue.ToString());
            res = CommandManager.DoCommand("? what");
            Assert.IsTrue(res.IsSucceed);
            res = CommandManager.DoCommand(string.Empty);
            Assert.IsTrue(res.IsSucceed);
            res = CommandManager.DoCommand("??");
            Assert.IsFalse(res.IsSucceed);
            

        }
        [TestMethod]
        public void TestReload()
        {
            var res = CommandManager.DoCommand("reload");
            Assert.IsTrue(res.IsSucceed);
            res = CommandManager.DoCommand("reload ./");
            Assert.IsTrue(res.IsSucceed);
            res = CommandManager.DoCommand("reload ?");
            Assert.IsTrue(res.IsSucceed);
            res = CommandManager.DoCommand("reload ver");
            Assert.IsTrue(res.IsSucceed);
        }
        [TestMethod]
        public void TestLoadAddons()
        {
            var res = CommandManager.DoCommand("test");
            Assert.IsTrue(res.IsSucceed);
        }
    }
}
