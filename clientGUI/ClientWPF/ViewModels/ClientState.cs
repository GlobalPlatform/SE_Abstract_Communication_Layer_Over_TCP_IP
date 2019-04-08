using System;
using System.Collections.Generic;

namespace ClientWPF.ViewModels
{
    public sealed class ClientState
    {
        private readonly String _name;
        private readonly int _value;

        public static readonly ClientState INSTANCIED = new ClientState(1, "INSTANCIED");
        public static readonly ClientState INITIALIZED = new ClientState(2, "INITIALIZED");
        public static readonly ClientState CONNECTED = new ClientState(3, "CONNECTED");
        public static readonly ClientState DISCONNECTED = new ClientState(4, "DISCONNECTED");

        private ClientState(int value, String name)
        {
            _name = name;
            _value = value;
        }

        public override String ToString()
        {
            return _name;
        }
    }
}
