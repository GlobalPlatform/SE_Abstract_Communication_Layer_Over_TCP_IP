namespace ServerWPF.ViewModels
{
    public class APIServerModel
    {
        private string _state;
        private string _ip;
        private string _port;

        public APIServerModel(ServerState state)
        {
            _state = state.ToString();
        }

        public APIServerModel(ServerState state, string ip, string port)
        {
            _state = state.ToString();
            _ip = ip;
            _port = port;
        }

        public string State
        {
            get => _state;
            set => _state = value;
        }

        public string ServerIP
        {
            get => _ip; 
            set => _ip = value; 
        }

        public string ServerPort
        {
            get => _port; 
            set => _port = value; 
        }
    }
}
