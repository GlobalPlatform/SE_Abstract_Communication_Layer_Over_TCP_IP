namespace ServerWPF.Services
{
    public interface IDialogService
    {
        void ShowMessageBox(string message);
        void ShowError(string message);
    }
}
