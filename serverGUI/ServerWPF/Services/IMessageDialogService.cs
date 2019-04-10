namespace ServerWPF.Services
{
    public interface IMessageDialogService
    {
        void ShowMessageBox(string message);
        void ShowError(string message);
    }
}
