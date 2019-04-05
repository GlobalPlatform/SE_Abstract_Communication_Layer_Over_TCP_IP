using ClientWPF.ViewModels;
using Newtonsoft.Json;
using ServerWPF.Models;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Input;

namespace ServerWPF.ViewModels
{
    class APIServerVM : ViewModelBase
    {
        private readonly int logsLimit = 100;
        private int logsPointer = 0;

        private APIServerWrapper _server;
        private string _currentCommand = "";

        public APIServerVM()
        {
            _connectionAccepted = new Callback(ConnectionAccepted);
            _server = new APIServerWrapper(_connectionAccepted);
            APIServerWrapper.InitServer();

            _startServer = new DelegateCommand(OnStartServer, null);
            _stopServer = new DelegateCommand(OnStopServer, null);
            _sendCommandClient = new DelegateCommand(OnSendCommandClient, IsClientSelected);
            _sendTypeA = new DelegateCommand(OnSendTypeA, IsClientSelected);
            _sendTypeB = new DelegateCommand(OnSendTypeB, IsClientSelected);
            _sendTypeF = new DelegateCommand(OnSendTypeF, IsClientSelected);
            _echoClient = new DelegateCommand(OnEchoClient, IsClientSelected);
            _diagnoseClient = new DelegateCommand(OnDiagnoseClient, IsClientSelected);
            _restartTarget = new DelegateCommand(OnRestartTarget, IsClientSelected);
            _stopClient = new DelegateCommand(OnStopClient, IsClientSelected);
            _clearLogs = new DelegateCommand(OnClearLogs, IsClientSelected);
            _coldReset = new DelegateCommand(OnColdReset, IsClientSelected);
            _warmReset = new DelegateCommand(OnWarmReset, IsClientSelected);
            _powerOFFField = new DelegateCommand(OnPowerOFFField, IsClientSelected);
            _powerONField = new DelegateCommand(OnPowerONField, IsClientSelected);
        }

        public string CurrentCommand
        {
            get => _currentCommand;
            set => SetProperty(ref _currentCommand, value);
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
            get => _serverData = _serverData ?? LoadServerData();
            set => SetProperty(ref _serverData, value);
        }

        private ObservableCollection<APIServerModel> LoadServerData()
        {
            ObservableCollection<APIServerModel> data = new ObservableCollection<APIServerModel>();
            string ip;
            string port;
            using (StreamReader r = new StreamReader("config/init.json"))
            {
                string json = r.ReadToEnd();
                dynamic array = JsonConvert.DeserializeObject(json);
                ip = array["ip"];
                port = array["port"];
            }
            data.Add(new APIServerModel("INITIALIZED", ip, port));
            ServerData = data;
            return ServerData;
        }

        public ObservableCollection<ClientModel> ClientsList
        {
            get => _clientsList = _clientsList ?? new ObservableCollection<ClientModel>();
            set => SetProperty(ref _clientsList, value);
        }

        public ClientModel SelectedClient
        {
            get => _selectedClient;
            set {
                SetProperty(ref _selectedClient, value);
                _sendCommandClient.InvokeCanExecuteChanged();
                _sendTypeA.InvokeCanExecuteChanged();
                _sendTypeB.InvokeCanExecuteChanged();
                _sendTypeF.InvokeCanExecuteChanged();
                _echoClient.InvokeCanExecuteChanged();
                _diagnoseClient.InvokeCanExecuteChanged();
                _restartTarget.InvokeCanExecuteChanged();
                _stopClient.InvokeCanExecuteChanged();
                _clearLogs.InvokeCanExecuteChanged();
                _coldReset.InvokeCanExecuteChanged();
                _warmReset.InvokeCanExecuteChanged();
                _powerOFFField.InvokeCanExecuteChanged();
                _powerONField.InvokeCanExecuteChanged();
            }
        }

        public ObservableCollection<LogModel> LogsList
        {
            get => _logsList = _logsList ?? new ObservableCollection<LogModel>();
            set => SetProperty(ref _logsList, value);
        }

        #endregion

        #region commands
        private readonly DelegateCommand _startServer;
        public ICommand StartServer => _startServer;

        private DelegateCommand _stopServer;
        public ICommand StopServer => _stopServer;

        private DelegateCommand _sendCommandClient;
        public ICommand SendCommandClient => _sendCommandClient;

        private DelegateCommand _sendTypeA;
        public ICommand SendTypeA => _sendTypeA;

        private DelegateCommand _sendTypeB;
        public ICommand SendTypeB => _sendTypeB;

        private DelegateCommand _sendTypeF;
        public ICommand SendTypeF => _sendTypeF;

        private DelegateCommand _echoClient;
        public ICommand EchoClient => _echoClient;

        private DelegateCommand _diagnoseClient;
        public ICommand DiagnoseClient => _diagnoseClient;

        private DelegateCommand _restartTarget;
        public ICommand RestartTarget => _restartTarget;

        private DelegateCommand _stopClient;
        public ICommand StopClient => _stopClient;

        private DelegateCommand _clearLogs;
        public ICommand ClearLogs => _clearLogs;

        private DelegateCommand _coldReset;
        public ICommand ColdReset => _coldReset;

        private DelegateCommand _warmReset;
        public ICommand WarmReset => _warmReset;

        private DelegateCommand _powerOFFField;
        public ICommand PowerOFFField => _powerOFFField;

        private DelegateCommand _powerONField;
        public ICommand PowerONField => _powerONField;

        private void OnStartServer(object commandParameter)
        {
            APIServerModel old = _serverData.FirstOrDefault();
            ResponseDLL response = APIServerWrapper.StartServer(old.ServerIP, old.ServerPort);
            if (CheckError(response)) return;
            _serverData.Clear();
            _serverData.Add(new APIServerModel("STARTED", old.ServerIP, old.ServerPort));
        }

        private void OnStopServer(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.StopServer();
            if (CheckError(response)) return;
            APIServerModel old = _serverData.FirstOrDefault();
            _serverData.Clear();
            _serverData.Add(new APIServerModel("INITIALIZED", old.ServerIP, old.ServerPort));
        }

        private void OnSendCommandClient(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.SendCommand(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("COMMAND {0}", _currentCommand), response);
            string[] sws = response.response.Split(' ');
            if (sws.Length == 2 && sws[0].Equals("61"))
            {
                response = APIServerWrapper.SendCommand(_selectedClient.ClientID, String.Format("00 C0 00 00 {0}", sws[1]));
                AppendLog(String.Format("COMMAND 00 C0 00 00 {0}", sws[1]), response);
            }
        }

        private void OnSendTypeA(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.SendTypeA(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("COMMAND {0}", _currentCommand), response);
        }

        private void OnSendTypeB(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.SendTypeB(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("COMMAND {0}", _currentCommand), response);
        }

        private void OnSendTypeF(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.SendTypeF(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("COMMAND {0}", _currentCommand), response);
        }

        private void OnEchoClient(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.EchoClient(_selectedClient.ClientID);
            AppendLog("IS ALIVE", response);
        }

        private void OnDiagnoseClient(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.DiagClient(_selectedClient.ClientID);
            AppendLog("DIAG", response);
        }

        private void OnColdReset(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.ColdReset(_selectedClient.ClientID);
            AppendLog("COLD RESET", response);
        }

        private void OnWarmReset(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.WarmReset(_selectedClient.ClientID);
            AppendLog("WARM RESET", response);
        }

        private void OnPowerOFFField(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.PowerOFFField(_selectedClient.ClientID);
            AppendLog("POWER OFF FIELD", response);
        }

        private void OnPowerONField(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.PowerONField(_selectedClient.ClientID);
            AppendLog("POWER ON FIELD", response);
        }

        private void OnRestartTarget(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.RestartTarget(_selectedClient.ClientID);
            AppendLog("RESTART", response);
        }

        private void OnStopClient(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.StopClient(_selectedClient.ClientID);
            AppendLog("STOP CLIENT", response);
            _clientsList.Remove(_selectedClient);
            _selectedClient = null;
        }

        private void OnClearLogs(object commandParameter)
        {
            _logsList.Clear();
        }

        #endregion

        private void AppendLog(string request, ResponseDLL response)
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

        private bool IsClientSelected(object commandParameter)
        {
            return _selectedClient != null;
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