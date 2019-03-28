using ServerWPF.Models;
using System;

namespace ClientWPF.ViewModels
{
    class LogModel
    {
        private int _idClient;
        private string _logTime;
        private string _nameClient;
        private string _request;
        private ResponseDLL _response;

        public LogModel(int idClient, string nameClient, string request, ResponseDLL response)
        {
            _idClient = idClient;
            _nameClient = nameClient;
            _request = request;
            _response = response;
            _logTime = DateTime.Now.ToString("s").Replace(":", ".");
        }

        public int IdClient
        {
            get { return _idClient; }
            set { _idClient = value; }
        }

        public string LogTime
        {
            get { return _logTime; }
            set { _logTime = value; }
        }

        public string NameClient
        {
            get { return _nameClient; }
            set { _nameClient = value; }
        }

        public string Request
        {
            get { return _request; }
            set { _request = value; }
        }

        public string Response
        {
            get { return _response.response; }
        }

        public int ErrServerCode
        {
            get { return _response.err_server_code; }
        }

        public string ErrServerDescription
        {
            get { return _response.err_server_description; }
        }

        public int ErrClientCode
        {
            get { return _response.err_client_code; }
        }

        public string ErrClientDescription
        {
            get { return _response.err_client_description; }
        }

        public int ErrTerminalCode
        {
            get { return _response.err_terminal_code; }
        }

        public string ErrTerminalDescription
        {
            get { return _response.err_terminal_description; }
        }

        public int ErrCardCode
        {
            get { return _response.err_card_code; }
        }

        public string ErrCardDescription
        {
            get { return _response.err_card_description; }
        }
    }
}