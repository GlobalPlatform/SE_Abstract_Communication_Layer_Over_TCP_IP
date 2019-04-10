using System;

namespace ServerWPF.Services
{
    public class FileDialogService : IFileDialogService
    {
        public string OpenFileDialog(string defaultPath)
        {
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            dlg.DefaultExt = ".txt";
            dlg.Filter = "TXT Files (*.txt)|*.txt";
            Nullable<bool> result = dlg.ShowDialog();
            return dlg.FileName;
        }
    }
}