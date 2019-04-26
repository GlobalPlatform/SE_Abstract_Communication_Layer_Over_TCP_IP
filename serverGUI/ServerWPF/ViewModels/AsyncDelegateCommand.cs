using System;
using System.Threading.Tasks;
using System.Windows.Input;

namespace ServerWPF.ViewModels
{
    public class AsyncDelegateCommand : ICommand
    {
        private Func<Task<bool>> _executeAction;
        private Func<bool> _canExecuteAction;

        public AsyncDelegateCommand(Func<Task<bool>> executeAction, Func<bool> canExecuteAction)
        {
            _executeAction = executeAction;
            _canExecuteAction = canExecuteAction;
        }

        public bool CanExecute(object parameter)
        {
            return _canExecuteAction?.Invoke() ?? true;
        }

        public event EventHandler CanExecuteChanged;

        public void Execute(object parameter)
        {
            _executeAction();
        }

        public void InvokeCanExecuteChanged()
        {
            CanExecuteChanged?.Invoke(this, EventArgs.Empty);
        }
    }
}
