using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerWPF.Services
{
    public interface IFileDialogService
    {
        string OpenFileDialog(string defaultPath);
    }
}
