using Newtonsoft.Json;
using ServerWPF.Services;
using ServerWPF.ViewModels;
using System.IO;
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

            string batch = "";
            using (StreamReader r = new StreamReader("config/init.json"))
            {
                string json = r.ReadToEnd();
                dynamic array = JsonConvert.DeserializeObject(json);
                batch = array["batch"];
            }
            if (batch != "")
            {
                current_file_path.Text = batch;
                apiServer.CurrentFilePath = batch;
            }
        }

        private void Btn_GetVersion_Click(object sender, RoutedEventArgs e)
        {
            txt_Version.Text = apiServer.DLL_GetVersion();
        }
    }
}
