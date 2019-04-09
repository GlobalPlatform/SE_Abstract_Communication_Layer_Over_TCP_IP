using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerWPF.Services
{
    public interface IDialogService
    {
        void ShowMessageBox(string message);
        void ShowError(string message);
    }
}
