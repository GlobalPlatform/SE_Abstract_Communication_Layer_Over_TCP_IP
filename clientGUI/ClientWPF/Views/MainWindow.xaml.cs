using ClientWPF.ViewModels;
using ServerWPF.Services;
using System.Windows;

namespace ClientWPF
{
    public partial class MainWindow : Window
    {
        private APIClientVM apiClient;

        public MainWindow()
        {
            apiClient = new APIClientVM(new DialogService());
            this.DataContext = apiClient;
            InitializeComponent();
            Style = (Style)FindResource(typeof(Window));
        }
    }
}
