using System;

namespace ClientWPF.ViewModels
{
    class APIClientModel
    {
        private string _state;
        private string _ip;
        private string _port;

        public APIClientModel(string state)
        {
            this._state = state;
        }

        public APIClientModel(string state, string ip, string port)
        {
            this._state = state;
            this._ip = ip;
            this._port = port;
        }

        public string ClientState
        {
            get { return _state; }
            set { _state = value; }
        }

        public string IpClientConnected
        {
            get { return _ip; }
            set { _ip = value; }
        }

        public string PortClientConnected
        {
            get { return _port; }
            set { _port = value; }
        }
    }
}
