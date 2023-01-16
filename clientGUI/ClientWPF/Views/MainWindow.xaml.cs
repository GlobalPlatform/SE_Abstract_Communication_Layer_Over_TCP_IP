using ClientWPF.ViewModels;
using Newtonsoft.Json;
using ServerWPF.Services;
using System.IO;
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

            string pcsc;
            using (StreamReader r = new StreamReader("config/init.json"))
            {
                string json = r.ReadToEnd();
                dynamic array = JsonConvert.DeserializeObject(json);
                pcsc = array["default_pcsc"];
            }

            if (pcsc != "")
            {
                for (int i = 0; i < dgReaders.Items.Count; i++)
                    if (dgReaders.Items.GetItemAt(i).ToString().Contains(pcsc))
                        dgReaders.SelectedIndex = i;
            }
        }
    }
}
