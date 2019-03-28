using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerWPF.ViewModels
{
    class APIServerModel
    {
        private string _state;
        private string _ip;
        private string _port;

        public APIServerModel(string state)
        {
            this._state = state;
        }

        public APIServerModel(string state, string ip, string port)
        {
            this._state = state;
            this._ip = ip;
            this._port = port;
        }

        public string ServerState
        {
            get { return _state; }
            set { _state = value; }
        }

        public string ServerIP
        {
            get { return _ip; }
            set { _ip = value; }
        }

        public string ServerPort
        {
            get { return _port; }
            set { _port = value; }
        }
    }
}
