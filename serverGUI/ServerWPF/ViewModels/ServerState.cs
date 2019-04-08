using System;
using System.Collections.Generic;

namespace ServerWPF.ViewModels
{
    public sealed class ServerState
    {
        private readonly String _name;
        private readonly int _value;

        public static readonly ServerState INSTANCIED = new ServerState(1, "INSTANCIED");
        public static readonly ServerState INITIALIZED = new ServerState(2, "INITIALIZED");
        public static readonly ServerState STARTED = new ServerState(3, "STARTED");
        public static readonly ServerState DISCONNECTED = new ServerState(4, "DISCONNECTED");

        private ServerState(int value, String name)
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
