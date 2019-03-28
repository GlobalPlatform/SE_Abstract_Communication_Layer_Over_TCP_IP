using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace ServerWPF.Models
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    struct ResponseDLL
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 2048)]
        public string response;

        public int err_server_code;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 2048)]
        public string err_server_description;

        public int err_client_code;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 2048)]
        public string err_client_description;

        public int err_terminal_code;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 2048)]
        public string err_terminal_description;

        public int err_card_code;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 2048)]
        public string err_card_description;
    }

    class APIServerWrapper
    {
        [DllImport(@"libs/libserver.dll")]
        static private extern IntPtr createServerAPI();

        [DllImport(@"libs/libserver.dll")]
        static private extern void initServer(IntPtr server, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void startServer(IntPtr server, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void listClients(IntPtr server, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void echoClient(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void diagClient(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void sendCommand(IntPtr server, int id_client, string command, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void restartTarget(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void stopClient(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void stopServer(IntPtr server, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void disposeServerAPI(IntPtr server);

        [DllImport(@"libs/libserver.dll")]
        static private extern void setCallbackConnectionAccepted(ViewModels.APIServerVM.Callback callback);

        static private IntPtr _server;

        public APIServerWrapper(ViewModels.APIServerVM.Callback connectionAccepted)
        {
            setCallbackConnectionAccepted(connectionAccepted);
            _server = createServerAPI();
        }

        static public ResponseDLL InitServer()
        {
            ResponseDLL response = new ResponseDLL();
            initServer(_server, ref response);
            return response;
        }

        static public ResponseDLL StartServer()
        {
            ResponseDLL response = new ResponseDLL();
            startServer(_server, ref response);
            return response;
        }

        static public ResponseDLL ListClients()
        {
            ResponseDLL response = new ResponseDLL();
            listClients(_server, ref response);
            return response;
        }

        static public ResponseDLL EchoClient(int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            echoClient(_server, id_client, ref response);
            return response;
        }

        static public ResponseDLL DiagClient(int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            diagClient(_server, id_client, ref response);
            return response;
        }

        static public ResponseDLL SendCommand(int id_client, string command)
        {
            ResponseDLL response = new ResponseDLL();
            sendCommand(_server, id_client, command, ref response);
            return response;
        }

        static public ResponseDLL RestartTarget(int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            restartTarget(_server, id_client, ref response);
            return response;
        }

        static public ResponseDLL StopClient(int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            stopClient(_server, id_client, ref response);
            return response;
        }

        static public ResponseDLL StopServer()
        {
            ResponseDLL response = new ResponseDLL();
            stopServer(_server, ref response);
            return response;
        }

        static public void DisposeServerAPI()
        {
            disposeServerAPI(_server);
        }
    }
}
