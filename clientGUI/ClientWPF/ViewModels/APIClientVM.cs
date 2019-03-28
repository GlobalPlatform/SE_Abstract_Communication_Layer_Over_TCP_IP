using Newtonsoft.Json;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows;
using WpfApplication1.ViewModels;

namespace ClientWPF.ViewModels
{
    class APIClientVM : INotifyPropertyChanged
    {
        private readonly int logsLimit = 100;
        private int logsPointer = 0;

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private APIClientWrapper _client;

        public APIClientVM()
        {
            _connectionLostCallback = new Callback(UpdateServerDown);
            _requestReceivedCallback = new Callback(UpdateRequestReceived);
            _responseSentCallback = new Callback(UpdateResponseSent);
            _client = new APIClientWrapper(_connectionLostCallback, _requestReceivedCallback, _responseSentCallback);
            APIClientWrapper.InitClient();
        }

        #region callbacks

        public delegate void Callback(string text);
        private Callback _connectionLostCallback;
        private Callback _requestReceivedCallback;
        private Callback _responseSentCallback;

        private void UpdateServerDown(string text)
        {
            if (_clientData.FirstOrDefault().ClientState.Equals("DISCONNECTED")) return;
            APIClientModel old = _clientData.FirstOrDefault();
            _clientData = new ObservableCollection<APIClientModel>();
            _clientData.Add(new APIClientModel("DISCONNECTED", old.IpClientConnected, old.PortClientConnected));
            OnPropertyChanged("ClientData");
        }

        private void UpdateRequestReceived(string text)
        {
            App.Current.Dispatcher.Invoke((Action)delegate
            {
                if (_logsList.Count == logsLimit)
                {
                    _logsList[logsPointer] = new LogModel("request", text);
                }
                else
                {
                    _logsList.Add(new LogModel("request", text));
                }
                logsPointer = (logsPointer + 1) % logsLimit;
            });
        }

        private void UpdateResponseSent(string text)
        {
            App.Current.Dispatcher.Invoke((Action)delegate
            {
                if (_logsList.Count == logsLimit)
                {
                    _logsList[logsPointer] = new LogModel("response", text); // circular insertion, oldest logs are removed
                }
                else
                {
                    _logsList.Add(new LogModel("response", text));
                }
                logsPointer = (logsPointer + 1) % logsLimit; 
            });
        }

        #endregion callbacks

        #region observable collections

        private ObservableCollection<APIClientModel> _clientData;
        private ObservableCollection<ReaderModel> _readersList;
        private ObservableCollection<LogModel> _logsList;
        private ReaderModel _selectedReader;

        public ObservableCollection<APIClientModel> ClientData
        {
            get { return _clientData = _clientData ?? LoadClientData(); }
        }
        private ObservableCollection<APIClientModel> LoadClientData()
        {
            _clientData = new ObservableCollection<APIClientModel>();
            string ip;
            string port;
            using (StreamReader r = new StreamReader("config/init.json"))
            {
                string json = r.ReadToEnd();
                dynamic array = JsonConvert.DeserializeObject(json);
                ip = array["ip"];
                port = array["port"];
            }
            _clientData.Add(new APIClientModel("INIITIALIZED", ip, port));
            return _clientData;
        }

        public ObservableCollection<ReaderModel> ReadersList
        {
            get { return _readersList = _readersList ?? LoadReaders(); }
        }
        private ObservableCollection<ReaderModel> LoadReaders()
        {
            ResponseDLL response = APIClientWrapper.LoadAndListReaders();
            _readersList = new ObservableCollection<ReaderModel>();
            if (CheckError(response)) return _readersList;
            String[] data = response.response.Split('|');

            for (int i = 0; i < data.Length - 1; i += 2)
            {
                _readersList.Add(new ReaderModel(i/2, data[i + 1]));
            }
            return _readersList;
        }
        public ReaderModel SelectedReader
        {
            get { return _selectedReader; }
            set { _selectedReader = value; }
        }

        public ObservableCollection<LogModel> LogsList
        {
            get { return _logsList = _logsList ?? new ObservableCollection<LogModel>(); }
        }

        #endregion observable collections

        #region commands

        private DelegateCommand _reloadReaders;
        private DelegateCommand _connectClient;
        private DelegateCommand _disconnectClient;
        private DelegateCommand _clearLogs;

        public DelegateCommand ReloadReaders
        {
            get { return _reloadReaders = _reloadReaders ?? new DelegateCommand(DelReloadReaders); }
        }
        private void DelReloadReaders()
        {
            _selectedReader = null;
            LoadReaders();
            OnPropertyChanged("ReadersList");
        }

        public DelegateCommand ConnectClient
        {
            get { return _connectClient = _connectClient ?? new DelegateCommand(DelConnectClient); }
        }
        private void DelConnectClient()
        {
            if (_selectedReader == null)
            {
                MessageBox.Show("Error: A reader must be selected");
                return;
            }
            APIClientModel old = _clientData.FirstOrDefault();
            ResponseDLL response = APIClientWrapper.ConnectClient(_selectedReader.ReaderName, old.IpClientConnected, old.PortClientConnected);
            if (CheckError(response)) return;
            _clientData = new ObservableCollection<APIClientModel>();
            _clientData.Add(new APIClientModel("CONNECTED", old.IpClientConnected, old.PortClientConnected));
            OnPropertyChanged("ClientData");
        }

        public DelegateCommand DisconnectClient
        {
            get { return _disconnectClient = _disconnectClient ?? new DelegateCommand(DelDisconnectClient); }
        }
        private void DelDisconnectClient()
        {
            APIClientModel old = _clientData.FirstOrDefault();
            if (old.ClientState.Equals("DISCONNECTED"))
            {
                MessageBox.Show("Error: Client already disconnected");
                return;
            }
            _clientData = new ObservableCollection<APIClientModel>();
            _clientData.Add(new APIClientModel("DISCONNECTED", old.IpClientConnected, old.PortClientConnected));
            OnPropertyChanged("ClientData");
            ResponseDLL response = APIClientWrapper.DisconnectClient();
            CheckError(response);
        }

        public DelegateCommand ClearLogs
        {
            get { return _clearLogs = _clearLogs ?? new DelegateCommand(DelClearLogs); }
        }
        private void DelClearLogs()
        {
            _logsList = new ObservableCollection<LogModel>();
            OnPropertyChanged("LogsList");
        }

        #endregion commands

        private bool CheckError(ResponseDLL packet)
        {
            bool hasError = false;
            string description = "none";
            if (packet.err_server_code != 0)
            {
                description = packet.err_server_description;
                hasError = true;
            }
            else if (packet.err_client_code != 0)
            {
                description = packet.err_client_description;
                hasError = true;
            }
            else if (packet.err_terminal_code != 0)
            {
                description = packet.err_terminal_description;
                hasError = true;
            }
            else if (packet.err_card_code != 0)
            {
                description = packet.err_card_description;
                hasError = true;
            }
            if (hasError) MessageBox.Show("Error: " + description);
            return hasError;
        }
    }
}