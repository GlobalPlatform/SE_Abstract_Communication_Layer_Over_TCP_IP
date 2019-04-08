namespace ClientWPF.ViewModels
{
    class APIClientModel
    {
        private string _state;
        private string _ip;
        private string _port;

        public APIClientModel(ClientState state)
        {
            _state = state.ToString();
        }

        public APIClientModel(ClientState state, string ip, string port)
        {
            _state = state.ToString();
            _ip = ip;
            _port = port;
        }

        public string ClientState
        {
            get =>_state; 
            set => _state = value; 
        }

        public string IpClientConnected
        {
            get =>_ip; 
            set => _ip = value; 
        }

        public string PortClientConnected
        {
            get => _port; 
            set => _port = value; 
        }
    }
}
