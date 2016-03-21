using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using NLua;
using System.Windows.Forms;
using System.Drawing;
namespace LuaTest
{
    [TestClass]
    public class LuaTest
    {
        static string testCode =
@"
import(""System"")
import(""System.Windows.Forms"")

Application.EnableVisualStyles()
Application.SetCompatibleTextRenderingDefault(false)

form = Form()
form:SuspendLayout()
form.Visible = true
form.Width=300
form.Height=200

clickTimes=0

function OnButtonClick(sender,e)
    clickTimes=clickTimes+1
    form.Text=Convert.ToString(clickTimes)
    if clickTimes>5 then
        MessageBox.Show(form,""Click"",""Hello World"") 
    end
end

button=Button()
button.Text=""Click Me""
button.Click:Add(OnButtonClick)

form.Controls:Add(button)
form:ResumeLayout(false)

Application.Run(form)

";
        [TestMethod]
        [STAThread]
        public void TestLua()
        {
            using (var lua = new Lua())
            {
                lua.LoadCLRPackage();
                lua.DoString(testCode);
            }
        }
    }
}
