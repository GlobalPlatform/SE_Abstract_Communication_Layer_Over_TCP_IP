using System;

namespace ClientWPF.ViewModels
{
    public class LogModel
    {
        private readonly string _type;
        private readonly string _data;
        private readonly string _time;

        public LogModel(string type, string data)
        {
            _type = type;
            _data = data;
            _time = DateTime.Now.ToString("s").Replace(":", ".");
        }

        public string LogType
        {
            get => _type;
        }

        public string LogData
        {
            get => _data;
        }

        public string LogTime
        {
            get => _time;
        }
    }
}