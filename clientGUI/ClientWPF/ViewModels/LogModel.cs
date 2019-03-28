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
            this._type = type;
            this._data = data;
            this._time = DateTime.Now.ToString("s").Replace(":", ".");
        }

        public string LogType
        {
            get
            {
                return _type;
            }
        }

        public string LogData
        {
            get
            {
                return _data;
            }
        }

        public string LogTime
        {
            get
            {
                return _time;
            }
        }
    }
}