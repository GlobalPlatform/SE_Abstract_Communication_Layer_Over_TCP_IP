using System;

namespace ClientWPF.ViewModels
{
    class ReaderModel
    {
        private readonly int _id;
        private readonly string _name;

        public ReaderModel(int id, string name)
        {
            this._id = id;
            this._name = name;
        }

        public int ReaderID
        {
            get
            {
                return _id;
            }
        }

        public string ReaderName
        {
            get
            {
                return _name;
            }
        }
    }
}
