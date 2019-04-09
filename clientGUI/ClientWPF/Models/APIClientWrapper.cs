using System;
using System.Runtime.InteropServices;
using static ClientWPF.ViewModels.APIClientVM;

namespace ClientWPF.ViewModels
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

    class APIClientWrapper
    {
        [DllImport(@"libs/libclient.dll")]
        static private extern IntPtr createClientAPI();

        [DllImport(@"libs/libclient.dll")]
        static private extern void disposeClientAPI(IntPtr client);

        [DllImport(@"libs/libclient.dll")]
        static private extern void disconnectClient(IntPtr client, ref ResponseDLL ptr);

        [DllImport(@"libs/libclient.dll")]
        static private extern void initClient(IntPtr client, ref ResponseDLL ptr);

        [DllImport(@"libs/libclient.dll")]
        static private extern void loadAndListReaders(IntPtr client, ref ResponseDLL ptr);

        [DllImport(@"libs/libclient.dll")]
        static private extern void connectClient(IntPtr client, string reader, string ip, string port, ref ResponseDLL ptr);

        [DllImport(@"libs/libclient.dll")]
        static private extern void setCallbackConnectionLost(Callback fn);

        [DllImport(@"libs/libclient.dll")]
        static private extern void setCallbackRequestsReceived(Callback fn);

        [DllImport(@"libs/libclient.dll")]
        static private extern void setCallbackResponseSent(Callback fn);

        static private IntPtr _client;

        public APIClientWrapper(Callback connectionLostCallback, Callback requestReceivedCallback, Callback responseSentCallback)
        {
            setCallbackConnectionLost(connectionLostCallback);
            setCallbackRequestsReceived(requestReceivedCallback);
            setCallbackResponseSent(responseSentCallback);
            _client = createClientAPI();
        }

        public static void DisposeClientAPI()
        {
            disposeClientAPI(_client);
        }

        public static ResponseDLL DisconnectClient()
        {
            ResponseDLL response = new ResponseDLL();
            disconnectClient(_client, ref response);
            return response;
        }

        public static ResponseDLL InitClient()
        {
            ResponseDLL response = new ResponseDLL();
            initClient(_client, ref response);
            return response;
        }

        public static ResponseDLL LoadAndListReaders()
        {
            ResponseDLL response = new ResponseDLL();
            loadAndListReaders(_client, ref response);
            return response;
        }

        public static ResponseDLL ConnectClient(string reader, string ip, string port)
        {
            ResponseDLL response = new ResponseDLL();
            connectClient(_client, reader, ip, port, ref response);
            return response;
        }

        public static string NO_ERROR = "NONE";
        public static string RetrieveErrorDescription(ResponseDLL packet)
        {
            string description = NO_ERROR;
            if (packet.err_server_code != 0)
            {
                description = packet.err_server_description;
            }
            else if (packet.err_client_code != 0)
            {
                description = packet.err_client_description;
            }
            else if (packet.err_terminal_code != 0)
            {
                description = packet.err_terminal_description;
            }
            else if (packet.err_card_code != 0)
            {
                description = packet.err_card_description;
            }
            return description;
        }
    }
}
