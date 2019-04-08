using System;
using System.Runtime.InteropServices;

namespace ServerWPF.Models
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct ResponseDLL
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

    public class APIServerWrapper
    {
        [DllImport(@"libs/libserver.dll")]
        static private extern IntPtr createServerAPI();

        [DllImport(@"libs/libserver.dll")]
        static private extern void initServer(IntPtr server, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void startServer(IntPtr server, string ip, string port, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void listClients(IntPtr server, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void echoClient(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void diagClient(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void sendCommand(IntPtr server, int id_client, string command, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void sendTypeA(IntPtr server, int id_client, string command, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void sendTypeB(IntPtr server, int id_client, string command, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void sendTypeF(IntPtr server, int id_client, string command, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void restartTarget(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void coldReset(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void warmReset(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void powerOFFField(IntPtr server, int id_client, ref ResponseDLL response_packet);

        [DllImport(@"libs/libserver.dll")]
        static private extern void powerONField(IntPtr server, int id_client, ref ResponseDLL response_packet);

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

        static public ResponseDLL StartServer(string ip, string port)
        {
            ResponseDLL response = new ResponseDLL();
            startServer(_server, ip, port, ref response);
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

        static public ResponseDLL SendTypeA(int id_client, string command)
        {
            ResponseDLL response = new ResponseDLL();
            sendTypeA(_server, id_client, command, ref response);
            return response;
        }

        static public ResponseDLL SendTypeB(int id_client, string command)
        {
            ResponseDLL response = new ResponseDLL();
            sendTypeB(_server, id_client, command, ref response);
            return response;
        }

        static public ResponseDLL SendTypeF(int id_client, string command)
        {
            ResponseDLL response = new ResponseDLL();
            sendTypeF(_server, id_client, command, ref response);
            return response;
        }

        static public ResponseDLL RestartTarget(int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            restartTarget(_server, id_client, ref response);
            return response;
        }

        static public ResponseDLL ColdReset(int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            coldReset(_server, id_client, ref response);
            return response;
        }

        static public ResponseDLL WarmReset( int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            warmReset(_server, id_client, ref response);
            return response;
        }

        static public ResponseDLL PowerOFFField(int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            powerOFFField(_server, id_client, ref response);
            return response;
        }

        static public ResponseDLL PowerONField(int id_client)
        {
            ResponseDLL response = new ResponseDLL();
            powerONField(_server, id_client, ref response);
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
