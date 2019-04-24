using Newtonsoft.Json;
using ServerWPF.Services;
using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Windows.Input;

namespace ClientWPF.ViewModels
{
    class APIClientVM : ViewModelBase
    {
        private readonly int logsLimit = 100;
        private int logsPointer = 0;

        private APIClientWrapper _client; 
        private readonly IDialogService _dialogService;

        public APIClientVM(IDialogService dialogService) // injects services
        {
            _dialogService = dialogService;

            _connectionLostCallback = new Callback(UpdateServerDown);
            _requestReceivedCallback = new Callback(UpdateRequestReceived);
            _responseSentCallback = new Callback(UpdateResponseSent);
            _client = new APIClientWrapper(_connectionLostCallback, _requestReceivedCallback, _responseSentCallback);

            _reloadReaders = new DelegateCommand(OnReloadReaders, null);
            _connectClient = new DelegateCommand(OnConnectClient, CanConnectClient);
            _disconnectClient = new DelegateCommand(OnDisconnectClient, CanDisconnectClient);
            _clearLogs = new DelegateCommand(OnClearLogs, null);

            CheckError(APIClientWrapper.InitClient());
        }

        #region callbacks
        public delegate void Callback(string text);
        private Callback _connectionLostCallback;
        private Callback _requestReceivedCallback;
        private Callback _responseSentCallback;

        private void UpdateServerDown(string text)
        {
            App.Current.Dispatcher.Invoke((Action)delegate
            {
                APIClientModel old = _clientData.FirstOrDefault();
                _clientData.Clear();
                _clientData.Add(new APIClientModel(ClientState.DISCONNECTED, old.IpClientConnected, old.PortClientConnected, old.Name));
                _disconnectClient.InvokeCanExecuteChanged();
                SelectedReader = null;
            });
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
            get =>_clientData = _clientData ?? LoadClientData(); 
            set => SetProperty(ref _clientData, value); 
        }

        private ObservableCollection<APIClientModel> LoadClientData()
        {
            _clientData = new ObservableCollection<APIClientModel>();
            string ip;
            string port;
            string name;
            using (StreamReader r = new StreamReader("config/init.json"))
            {
                string json = r.ReadToEnd();
                dynamic array = JsonConvert.DeserializeObject(json);
                ip = array["ip"];
                port = array["port"];
                name = array["name"];
            }
            _clientData.Add(new APIClientModel(ClientState.INITIALIZED, ip, port, name));
            return _clientData;
        }

        public ObservableCollection<ReaderModel> ReadersList
        {
            get { return _readersList = _readersList ?? LoadReaders(); }
            set { SetProperty(ref _readersList, value); }
        }

        private ObservableCollection<ReaderModel> LoadReaders()
        {
            ResponseDLL response = APIClientWrapper.LoadAndListReaders();
            _readersList = _readersList ?? new ObservableCollection<ReaderModel>();
            _readersList.Clear();
            String[] data = response.response.Split('|');
            for (int i = 0; i < data.Length - 1; i += 2)
            {
                _readersList.Add(new ReaderModel(i / 2, data[i + 1]));
            }
            return _readersList;
        }

        public ReaderModel SelectedReader
        {
            get => _selectedReader;
            set
            {
                SetProperty(ref _selectedReader, value);
                _connectClient.InvokeCanExecuteChanged();
            }
        }

        public ObservableCollection<LogModel> LogsList
        {
            get => _logsList = _logsList ?? new ObservableCollection<LogModel>(); 
        }
        #endregion observable collections

        #region commands
        private readonly DelegateCommand _reloadReaders;
        public ICommand ReloadReaders => _reloadReaders;

        private readonly DelegateCommand _connectClient;
        public ICommand ConnectClient => _connectClient;

        private readonly DelegateCommand _disconnectClient;
        public ICommand DisconnectClient => _disconnectClient;

        private readonly DelegateCommand _clearLogs;
        public ICommand ClearLogs => _clearLogs;

        private void OnReloadReaders()
        {
            SelectedReader = null;
            LoadReaders();
        }

        private void OnConnectClient()
        {
            APIClientModel old = _clientData.FirstOrDefault();
            ResponseDLL response = APIClientWrapper.ConnectClient(_selectedReader.ReaderName, old.IpClientConnected, old.PortClientConnected);
            if (CheckError(response)) return;
            _clientData.Clear();
            _clientData.Add(new APIClientModel(ClientState.CONNECTED, old.IpClientConnected, old.PortClientConnected, old.Name));
            _connectClient.InvokeCanExecuteChanged();
            _disconnectClient.InvokeCanExecuteChanged();
        }

        private void OnDisconnectClient()
        {
            ResponseDLL response = APIClientWrapper.DisconnectClient();
            if (CheckError(response)) return;
            APIClientModel old = _clientData.FirstOrDefault();
            _clientData.Clear();
            _clientData.Add(new APIClientModel(ClientState.DISCONNECTED, old.IpClientConnected, old.PortClientConnected, old.Name));
            SelectedReader = null;
            _connectClient.InvokeCanExecuteChanged();
            _disconnectClient.InvokeCanExecuteChanged();
        }

        private void OnClearLogs()
        {
            _logsList.Clear();
        }

        private bool CanConnectClient()
        {
            APIClientModel client = _clientData.FirstOrDefault();
            return !client.ClientState.Equals(ClientState.CONNECTED.ToString()) && _selectedReader != null;
        }

        private bool CanDisconnectClient()
        {
            APIClientModel client = _clientData.FirstOrDefault();
            return client.ClientState.Equals(ClientState.CONNECTED.ToString());
        }
        #endregion commands

        #region utils
        private bool CheckError(ResponseDLL packet)
        {
            string description = APIClientWrapper.RetrieveErrorDescription(packet);
            if (description.Equals(APIClientWrapper.NO_ERROR)) return false;
             _dialogService.ShowError(description);
            return true;
        }
        #endregion
    }
}