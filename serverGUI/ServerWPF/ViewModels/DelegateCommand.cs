using System;
using System.Windows.Input;

namespace ServerWPF.ViewModels
{
    public class DelegateCommand : ICommand
    {
        private Func<bool> _executeAction;
        private Func<bool> _canExecuteAction;

        public DelegateCommand(Func<bool> executeAction, Func<bool> canExecuteAction)
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
