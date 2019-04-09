using System.Windows;

namespace ServerWPF.Services
{
    class DialogService : IDialogService
    {
        public void ShowError(string message)
        {
            MessageBox.Show(message, "An error has occurred...", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        public void ShowMessageBox(string message)
        {
            MessageBox.Show(message);
        }
    }
}
