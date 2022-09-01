using System;
using System.Threading;
using System.Windows.Forms;

namespace UI
{
    class Functions
    {
        public static string exploitdllname = "SimpleExecutor.dll";
        public static string racbwdllname = "h2FGh45fghdGDF.dll";
        public static void Inject()
        {
            if (NamedPipes.NamedPipeExist(NamedPipes.luapipename))
            {
                MessageBox.Show("Already injected!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
                return;
            }
            else if (!NamedPipes.NamedPipeExist(NamedPipes.luapipename))
            {
                switch (Injector.DllInjector.GetInstance.Inject("RobloxPlayerBeta", "", AppDomain.CurrentDomain.BaseDirectory + "/bin/" + racbwdllname))
                {
                    case Injector.DllInjectionResult.DllNotFound:
                        MessageBox.Show($"Couldn't find {exploitdllname}", "Dll was not found!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    case Injector.DllInjectionResult.GameProcessNotFound:
                        MessageBox.Show("Couldn't find RobloxPlayerBeta.exe!", "Target process was not found!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    case Injector.DllInjectionResult.InjectionFailed:
                        MessageBox.Show("Injection Failed!", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                }

                switch (Injector.DllInjector.GetInstance.Inject("RobloxPlayerBeta", "Roblox", AppDomain.CurrentDomain.BaseDirectory + "/bin/" + exploitdllname))
                {
                    case Injector.DllInjectionResult.DllNotFound:
                        MessageBox.Show($"Couldn't find {exploitdllname}", "Dll was not found!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    case Injector.DllInjectionResult.GameProcessNotFound:
                        MessageBox.Show("Couldn't find RobloxPlayerBeta.exe!", "Target process was not found!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    case Injector.DllInjectionResult.InjectionFailed:
                        MessageBox.Show("Injection Failed!", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                }

                int tries = 0;
                while (!NamedPipes.NamedPipeExist(NamedPipes.luapipename) || tries > 10) {
                    tries++;
                    Thread.Sleep(100);
                }

                if (!NamedPipes.NamedPipeExist(NamedPipes.luapipename)) {
                    MessageBox.Show("Injection Failed!\nMaybe you are Missing something\nor took more time to check if was ready\nor other stuff", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }
    }
}