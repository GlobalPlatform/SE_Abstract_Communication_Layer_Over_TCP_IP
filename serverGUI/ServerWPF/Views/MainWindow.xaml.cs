using ServerWPF.Services;
using ServerWPF.ViewModels;
using System.Windows;

namespace ServerWPF
{
    public partial class MainWindow : Window
    {
        private APIServerVM apiServer;

        public MainWindow()
        {
            apiServer = new APIServerVM(new MessageDialogService(), new FileDialogService());
            this.DataContext = apiServer;
            InitializeComponent();
            Style = (Style)FindResource(typeof(Window));
        }
    }
}
