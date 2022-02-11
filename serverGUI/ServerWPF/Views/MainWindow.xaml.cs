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

        private void Button_Click(object sender, RoutedEventArgs e)
        {

        }

        private void Btn_GetVersion_Click(object sender, RoutedEventArgs e)
        {
            txt_Version.Text = apiServer.DLL_GetVersion();
        }
    }
}
