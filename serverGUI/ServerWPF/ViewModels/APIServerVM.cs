using ClientWPF.ViewModels;
using Newtonsoft.Json;
using ServerWPF.Models;
using ServerWPF.Services;
using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Input;

namespace ServerWPF.ViewModels
{
    public class APIServerVM : ViewModelBase
    {
        private readonly int logsLimit = 100;
        private int logsPointer = 0;

        private APIServerWrapper _server;
        private ActionFlyweight _actions = new ActionFlyweight();

        private readonly IMessageDialogService _messageDialogService;
        private readonly IFileDialogService _fileDialogService;

        public APIServerVM(IMessageDialogService dialogService, IFileDialogService fileDialogService) // injects services
        {
            _messageDialogService = dialogService;
            _fileDialogService = fileDialogService;

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
            _browseFile = new DelegateCommand(OnBrowseFile, IsClientSelected);
            _sendCommandsBatch = new DelegateCommand(async () => await OnSendCommandBatch(), CanSendCommandBatch);

            _actions.AddAction(ActionMethod.COMMAND.ToString(), OnSendCommandClient);
            _actions.AddAction(ActionMethod.SEND_TYPE_A.ToString(), OnSendTypeA);
            _actions.AddAction(ActionMethod.SEND_TYPE_B.ToString(), OnSendTypeB);
            _actions.AddAction(ActionMethod.SEND_TYPE_F.ToString(), OnSendTypeF);
            _actions.AddAction(ActionMethod.ECHO.ToString(), OnEchoClient);
            _actions.AddAction(ActionMethod.DIAG.ToString(), OnDiagnoseClient);
            _actions.AddAction(ActionMethod.RESTART.ToString(), OnRestartTarget);
            _actions.AddAction(ActionMethod.STOP_CLIENT.ToString(), OnStopClient);
            _actions.AddAction(ActionMethod.COLD_RESET.ToString(), OnColdReset);
            _actions.AddAction(ActionMethod.WARM_RESET.ToString(), OnWarmReset);
            _actions.AddAction(ActionMethod.POWER_OFF_FIELD.ToString(), OnPowerOFFField);
            _actions.AddAction(ActionMethod.POWER_ON_FIELD.ToString(), OnPowerONField);

            LoadServerData();
            APIServerWrapper.InitServer();
        }

        private string _currentCommand = "";
        public string CurrentCommand
        {
            get => _currentCommand;
            set => SetProperty(ref _currentCommand, value);
        }

        private string _currentFilePath = "";
        public string CurrentFilePath
        {
            get => _currentFilePath;
            set
            {
                SetProperty(ref _currentFilePath, value);
                _sendCommandsBatch.InvokeCanExecuteChanged();
            }
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
            set
            {
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
                _browseFile.InvokeCanExecuteChanged();
                _sendCommandsBatch.InvokeCanExecuteChanged();
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

        private readonly DelegateCommand _browseFile;
        public ICommand BrowseFile => _browseFile;

        private readonly DelegateCommand _sendCommandsBatch;
        public ICommand SendCommandsBatch => _sendCommandsBatch;
        #endregion

        #region delegates implementations
        private void OnStartServer()
        {
            APIServerModel old = _serverData.FirstOrDefault();
            ResponseDLL response = APIServerWrapper.StartServer(old.ServerIP, old.ServerPort);
            if (CheckError(response)) return;
            _serverData.Clear();
            _serverData.Add(new APIServerModel(ServerState.STARTED, old.ServerIP, old.ServerPort));
            _startServer.InvokeCanExecuteChanged();
            _stopServer.InvokeCanExecuteChanged();
        }

        private void OnStopServer()
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

        private void OnSendCommandClient()
        {
            ResponseDLL response = APIServerWrapper.SendCommand(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("{0} {1}", ActionMethod.COMMAND.ToString(), _currentCommand), response);
            string[] sws = response.response.Split(' ');
            if (sws.Length == 2 && sws[0].Equals("61"))
            {
                response = APIServerWrapper.SendCommand(_selectedClient.ClientID, String.Format("00 C0 00 00 {0}", sws[1]));
                AppendLog(String.Format("{0} 00 C0 00 00 {1}", ActionMethod.COMMAND.ToString(), sws[1]), response);
            }
        }

        private void OnSendTypeA()
        {
            ResponseDLL response = APIServerWrapper.SendTypeA(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("{0} {1}", ActionMethod.SEND_TYPE_A.ToString(), _currentCommand), response);
        }

        private void OnSendTypeB()
        {
            ResponseDLL response = APIServerWrapper.SendTypeB(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("{0} {1}", ActionMethod.SEND_TYPE_B.ToString(), _currentCommand), response);
        }

        private void OnSendTypeF()
        {
            ResponseDLL response = APIServerWrapper.SendTypeF(_selectedClient.ClientID, _currentCommand);
            AppendLog(String.Format("{0} {1}", ActionMethod.SEND_TYPE_F.ToString(), _currentCommand), response);
        }

        private void OnEchoClient()
        {
            ResponseDLL response = APIServerWrapper.EchoClient(_selectedClient.ClientID);
            AppendLog(ActionMethod.ECHO.ToString(), response);
        }

        private void OnDiagnoseClient()
        {
            ResponseDLL response = APIServerWrapper.DiagClient(_selectedClient.ClientID);
            AppendLog(ActionMethod.DIAG.ToString(), response);
        }

        private void OnColdReset()
        {
            ResponseDLL response = APIServerWrapper.ColdReset(_selectedClient.ClientID);
            AppendLog(ActionMethod.COLD_RESET.ToString(), response);
        }

        private void OnWarmReset()
        {
            ResponseDLL response = APIServerWrapper.WarmReset(_selectedClient.ClientID);
            AppendLog(ActionMethod.WARM_RESET.ToString(), response);
        }

        private void OnPowerOFFField()
        {
            ResponseDLL response = APIServerWrapper.PowerOFFField(_selectedClient.ClientID);
            AppendLog(ActionMethod.POWER_OFF_FIELD.ToString(), response);
        }

        private void OnPowerONField()
        {
            ResponseDLL response = APIServerWrapper.PowerONField(_selectedClient.ClientID);
            AppendLog(ActionMethod.POWER_ON_FIELD.ToString(), response);
        }

        private void OnRestartTarget()
        {
            ResponseDLL response = APIServerWrapper.RestartTarget(_selectedClient.ClientID);
            AppendLog(ActionMethod.RESTART.ToString(), response);
        }

        private void OnStopClient()
        {
            ResponseDLL response = APIServerWrapper.StopClient(_selectedClient.ClientID);
            AppendLog(ActionMethod.STOP_CLIENT.ToString(), response);
            _clientsList.Remove(_selectedClient);
            _selectedClient = null;
        }

        private void OnClearLogs()
        {
            _logsList.Clear();
        }

        private void OnBrowseFile()
        {
            CurrentFilePath = _fileDialogService.OpenFileDialog(@"C:\Users\st\Documents\GitHub\SE_Abstract_Communication_Layer_Over_TCP_IP\clientGUI\ClientWPF\bin\Debug") ?? String.Empty;
        }

        private async Task OnSendCommandBatch()
        {
            var lines = File.ReadLines(CurrentFilePath);
            foreach (string line in lines)
            {
                if (SelectedClient == null) break;
                string[] tokens = line.Split(' ');
                if (tokens.Length == 2) CurrentCommand = tokens[1];
                Action action = _actions.GetAction(tokens[0]);
                if (action != null)
                {
                    await Task.Run(action);
                }
            }
        }

        private void AppendLog(string request, ResponseDLL response)
        {
            App.Current.Dispatcher.Invoke((Action)delegate
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
            });
        }

        private bool IsClientSelected()
        {
            return _selectedClient != null;
        }

        private bool CanStartServer()
        {
            APIServerModel client = _serverData.FirstOrDefault();
            return client.State.Equals(ServerState.DISCONNECTED.ToString()) || client.State.Equals(ServerState.INITIALIZED.ToString());
        }

        private bool CanStopServer()
        {
            APIServerModel client = _serverData.FirstOrDefault();
            return client != null && client.State.Equals(ServerState.STARTED.ToString());
        }

        private bool CanSendCommandBatch()
        {
            return SelectedClient != null && File.Exists(CurrentFilePath);
        }
        #endregion

        #region utils
        private bool CheckError(ResponseDLL packet)
        {
            string description = APIServerWrapper.RetrieveErrorDescription(packet);
            if (description.Equals(APIServerWrapper.NO_ERROR)) return false;
            _messageDialogService.ShowError(description);
            return true;
        }
        #endregion
    }
}