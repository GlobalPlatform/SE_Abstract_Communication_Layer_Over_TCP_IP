using ClientWPF.ViewModels;
using Newtonsoft.Json;
using ServerWPF.Models;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows;

namespace ServerWPF.ViewModels
{
    class APIServerVM : INotifyPropertyChanged
    {
        private readonly int logsLimit = 100;
        private int logsPointer = 0;

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private APIServerWrapper _server;
        private string _currentCommand = "";

        public APIServerVM()
        {
            _connectionAccepted = new Callback(ConnectionAccepted);
            _server = new APIServerWrapper(_connectionAccepted);
            APIServerWrapper.InitServer();
        }

        public string CurrentCommand
        {
            get { return _currentCommand; }
            set { _currentCommand = value; }
        }

        #region callbacks

        public delegate void Callback(int id, string name);
        private Callback _connectionAccepted;

        private void ConnectionAccepted(int id, string name)
        {
            App.Current.Dispatcher.Invoke((Action)delegate
            {
                _clientsList.Add(new ClientModel(id, name));
            });
        }

        #endregion

        #region observable collections

        private ObservableCollection<APIServerModel> _serverData;
        private ObservableCollection<ClientModel> _clientsList;
        private ObservableCollection<LogModel> _logsList;
        private ClientModel _selectedClient;

        public ObservableCollection<APIServerModel> ServerData
        {
            get { return _serverData = _serverData ?? LoadServerData(); }
        }

        private ObservableCollection<APIServerModel> LoadServerData()
        {
            _serverData = new ObservableCollection<APIServerModel>();
            string ip;
            string port;
            using (StreamReader r = new StreamReader("config/init.json"))
            {
                string json = r.ReadToEnd();
                dynamic array = JsonConvert.DeserializeObject(json);
                ip = array["ip"];
                port = array["port"];
            }
            _serverData.Add(new APIServerModel("INIITIALIZED", ip, port));
            return _serverData;
        }

        public ObservableCollection<ClientModel> ClientsList
        {
            get { return _clientsList = _clientsList ?? LoadClients(); }
        }

        private ObservableCollection<ClientModel> LoadClients()
        {
            _clientsList = new ObservableCollection<ClientModel>();
            return _clientsList;
        }

        public ClientModel SelectedClient
        {
            get { return _selectedClient; }
            set { _selectedClient = value; }
        }

        public ObservableCollection<LogModel> LogsList
        {
            get { return _logsList = _logsList ?? new ObservableCollection<LogModel>(); }
        }

        #endregion

        #region commands

        private DelegateCommand _startServer;

        public DelegateCommand StartServer
        {
            get { return _startServer = _startServer ?? new DelegateCommand(DelStartServer); }
        }

        private void DelStartServer()
        {
            APIServerWrapper.StartServer();
            APIServerModel old = _serverData.FirstOrDefault();
            _serverData = new ObservableCollection<APIServerModel>();
            _serverData.Add(new APIServerModel("STARTED", old.ServerIP, old.ServerPort));
            OnPropertyChanged("ServerData");
        }

        private DelegateCommand _stopServer;

        public DelegateCommand StopServer
        {
            get { return _stopServer = _stopServer ?? new DelegateCommand(DelStopServer); }
        }

        private void DelStopServer()
        {
            APIServerWrapper.StopServer();
            APIServerModel old = _serverData.FirstOrDefault();
            _serverData = new ObservableCollection<APIServerModel>();
            _serverData.Add(new APIServerModel("INITIALIZED", old.ServerIP, old.ServerPort));
            OnPropertyChanged("ServerData");
        }

        private DelegateCommand _sendCommandClient;

        public DelegateCommand SendCommandClient
        {
            get { return _sendCommandClient = _sendCommandClient ?? new DelegateCommand(DelSendCommandClient); }
        }

        private void DelSendCommandClient()
        {
            if (_selectedClient == null)
            {
                MessageBox.Show("Erreur: A client must be selected.");
                return;
            }
            ResponseDLL response = APIServerWrapper.SendCommand(_selectedClient.ClientID, _currentCommand);
            AddNewLog(String.Format("COMMAND {0}", _currentCommand), response);
        }

        private DelegateCommand _echoClient;

        public DelegateCommand EchoClient
        {
            get { return _echoClient = _echoClient ?? new DelegateCommand(DelEchoClient); }
        }

        private void DelEchoClient()
        {
            if (_selectedClient == null)
            {
                MessageBox.Show("Erreur: A client must be selected.");
                return;
            }
            ResponseDLL response = APIServerWrapper.EchoClient(_selectedClient.ClientID);
            AddNewLog("IS ALIVE", response);
        }

        private DelegateCommand _diagnoseClient;

        public DelegateCommand DiagnoseClient
        {
            get { return _diagnoseClient = _diagnoseClient ?? new DelegateCommand(DelDiagnoseClient); }
        }

        private void DelDiagnoseClient()
        {
            if (_selectedClient == null)
            {
                MessageBox.Show("Erreur: A client must be selected.");
                return;
            }
            ResponseDLL response = APIServerWrapper.DiagClient(_selectedClient.ClientID);
            AddNewLog("DIAG", response);
        }

        private DelegateCommand _restartTarget;

        public DelegateCommand RestartTarget
        {
            get { return _restartTarget = _restartTarget ?? new DelegateCommand(DelRestartTarget); }
        }

        private void DelRestartTarget()
        {
            if (_selectedClient == null)
            {
                MessageBox.Show("Erreur: A client must be selected.");
                return;
            }
            ResponseDLL response = APIServerWrapper.RestartTarget(_selectedClient.ClientID);
            AddNewLog("RESTART", response);
        }

        private DelegateCommand _stopClient;

        public DelegateCommand StopClient
        {
            get { return _stopClient = _stopClient ?? new DelegateCommand(DelStopClient); }
        }

        private void DelStopClient()
        {
            if (_selectedClient == null)
            {
                MessageBox.Show("Erreur: A client must be selected.");
                return;
            }
            ResponseDLL response = APIServerWrapper.StopClient(_selectedClient.ClientID);
            AddNewLog("STOP CLIENT", response);
            _clientsList.Remove(_selectedClient);
            _selectedClient = null;
            OnPropertyChanged("ClientsList");
        }

        private DelegateCommand _clearLogs;

        public DelegateCommand ClearLogs
        {
            get { return _clearLogs = _clearLogs ?? new DelegateCommand(DelClearLogs); }
        }

        private void DelClearLogs()
        {
            _logsList = new ObservableCollection<LogModel>();
            OnPropertyChanged("LogsList");
        }

        #endregion

        private void AddNewLog(string request, ResponseDLL response)
        {
            if (_logsList.Count == logsLimit)
            {
                _logsList[logsPointer] = new LogModel(_selectedClient.ClientID, _selectedClient.ClientName, request, response);
            }
            else
            {
                _logsList.Add(new LogModel(_selectedClient.ClientID, _selectedClient.ClientName, request, response));
            }
            logsPointer = (logsPointer + 1) % logsLimit;
        }

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