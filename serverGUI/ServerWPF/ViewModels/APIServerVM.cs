using ClientWPF.ViewModels;
using Newtonsoft.Json;
using ServerWPF.Models;
using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Input;

namespace ServerWPF.ViewModels
{
    public class APIServerVM : ViewModelBase
    {
        private readonly int logsLimit = 100;
        private int logsPointer = 0;

        private APIServerWrapper _server;
        private string _currentCommand = "";

        public APIServerVM()
        {
            _connectionAccepted = new Callback(ConnectionAccepted);
            _server = new APIServerWrapper(_connectionAccepted);

            _startServer = new DelegateCommand(OnStartServer, CanStartServer);
            _stopServer = new DelegateCommand(OnStopServer, CanStopServer);
            _sendCommandClient = new DelegateCommand(OnSendCommandClient, IsClientSelected);
            _sendTypeA = new DelegateCommand(OnSendTypeA, IsClientSelected);
            _sendTypeB = new DelegateCommand(OnSendTypeB, IsClientSelected);
            _sendTypeF = new DelegateCommand(OnSendTypeF, IsClientSelected);
            _echoClient = new DelegateCommand(OnEchoClient, IsClientSelected);
            _diagnoseClient = new DelegateCommand(OnDiagnoseClient, IsClientSelected);
            _restartTarget = new DelegateCommand(OnRestartTarget, IsClientSelected);
            _stopClient = new DelegateCommand(OnStopClient, IsClientSelected);
            _coldReset = new DelegateCommand(OnColdReset, IsClientSelected);
            _warmReset = new DelegateCommand(OnWarmReset, IsClientSelected);
            _powerOFFField = new DelegateCommand(OnPowerOFFField, IsClientSelected);
            _powerONField = new DelegateCommand(OnPowerONField, IsClientSelected);
            _clearLogs = new DelegateCommand(OnClearLogs, null);

            LoadServerData();
            APIServerWrapper.InitServer();
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
            _serverData.Add(new APIServerModel(ServerState.INITIALIZED, ip, port));
            return _serverData;
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

        #region delegates declarations
        private readonly DelegateCommand _startServer;
        public ICommand StartServer => _startServer;

        private readonly DelegateCommand _stopServer;
        public ICommand StopServer => _stopServer;

        private readonly DelegateCommand _sendCommandClient;
        public ICommand SendCommandClient => _sendCommandClient;

        private readonly DelegateCommand _sendTypeA;
        public ICommand SendTypeA => _sendTypeA;

        private readonly DelegateCommand _sendTypeB;
        public ICommand SendTypeB => _sendTypeB;

        private readonly DelegateCommand _sendTypeF;
        public ICommand SendTypeF => _sendTypeF;

        private readonly DelegateCommand _echoClient;
        public ICommand EchoClient => _echoClient;

        private readonly DelegateCommand _diagnoseClient;
        public ICommand DiagnoseClient => _diagnoseClient;

        private readonly DelegateCommand _restartTarget;
        public ICommand RestartTarget => _restartTarget;

        private readonly DelegateCommand _stopClient;
        public ICommand StopClient => _stopClient;

        private readonly DelegateCommand _clearLogs;
        public ICommand ClearLogs => _clearLogs;

        private readonly DelegateCommand _coldReset;
        public ICommand ColdReset => _coldReset;

        private readonly DelegateCommand _warmReset;
        public ICommand WarmReset => _warmReset;

        private readonly DelegateCommand _powerOFFField;
        public ICommand PowerOFFField => _powerOFFField;

        private readonly DelegateCommand _powerONField;
        public ICommand PowerONField => _powerONField;
        #endregion

        #region delegates implementations
        private void OnStartServer(object commandParameter)
        {
            APIServerModel old = _serverData.FirstOrDefault();
            ResponseDLL response = APIServerWrapper.StartServer(old.ServerIP, old.ServerPort);
            if (CheckError(response)) return;
            _serverData.Clear();
            _serverData.Add(new APIServerModel(ServerState.STARTED, old.ServerIP, old.ServerPort));
            _startServer.InvokeCanExecuteChanged();
            _stopServer.InvokeCanExecuteChanged();
        }

        private void OnStopServer(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.StopServer();
            if (CheckError(response)) return;
            APIServerModel old = _serverData.FirstOrDefault();
            _serverData.Clear();
            _serverData.Add(new APIServerModel(ServerState.DISCONNECTED, old.ServerIP, old.ServerPort));
            _clientsList.Clear();
            _startServer.InvokeCanExecuteChanged();
            _stopServer.InvokeCanExecuteChanged();
        }

        private void OnSendCommandClient(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.SendCommand(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("{0} {1}", ActionMethod.COMMAND.ToString(), _currentCommand), response);
            string[] sws = response.response.Split(' ');
            if (sws.Length == 2 && sws[0].Equals("61"))
            {
                AppendLog(String.Format("{0} 00 C0 00 00 {1}", ActionMethod.COMMAND.ToString(), sws[1]), response);
            }
        }

        private void OnSendTypeA(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.SendTypeA(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("{0} {1}", ActionMethod.SEND_TYPE_A.ToString(), _currentCommand), response);
        }

        private void OnSendTypeB(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.SendTypeB(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("{0} {1}", ActionMethod.SEND_TYPE_B.ToString(), _currentCommand), response);
        }

        private void OnSendTypeF(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.SendTypeF(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("{0} {1}", ActionMethod.SEND_TYPE_F.ToString(), _currentCommand), response);
        }

        private void OnEchoClient(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.EchoClient(_selectedClient.ClientID);
            AppendLog(ActionMethod.ECHO.ToString(), response);
        }

        private void OnDiagnoseClient(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.DiagClient(_selectedClient.ClientID);
            AppendLog(ActionMethod.DIAG.ToString(), response);
        }

        private void OnColdReset(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.ColdReset(_selectedClient.ClientID);
            AppendLog(ActionMethod.COLD_RESET.ToString(), response);
        }

        private void OnWarmReset(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.WarmReset(_selectedClient.ClientID);
            AppendLog(ActionMethod.WARM_RESET.ToString(), response);
        }

        private void OnPowerOFFField(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.PowerOFFField(_selectedClient.ClientID);
            AppendLog(ActionMethod.POWER_OFF_FIELD.ToString(), response);
        }

        private void OnPowerONField(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.PowerONField(_selectedClient.ClientID);
            AppendLog(ActionMethod.POWER_ON_FIELD.ToString(), response);
        }

        private void OnRestartTarget(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.RestartTarget(_selectedClient.ClientID);
            AppendLog(ActionMethod.RESTART.ToString(), response);
        }

        private void OnStopClient(object commandParameter)
        {
            ResponseDLL response = APIServerWrapper.StopClient(_selectedClient.ClientID);
            AppendLog(ActionMethod.STOP_CLIENT.ToString(), response);
            _clientsList.Remove(_selectedClient);
            _selectedClient = null;
        }

        private void OnClearLogs(object commandParameter)
        {
            _logsList.Clear();
        }

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

        private bool CanStartServer(object commandParameter)
        {
            APIServerModel client = _serverData.FirstOrDefault();
            return client.State.Equals(ServerState.DISCONNECTED.ToString()) || client.State.Equals(ServerState.INITIALIZED.ToString());
        }

        private bool CanStopServer(object commandParameter)
        {
            APIServerModel client = _serverData.FirstOrDefault();
            return client != null && client.State.Equals(ServerState.STARTED.ToString());
        }
        #endregion

        #region utils
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
        #endregion
    }
}