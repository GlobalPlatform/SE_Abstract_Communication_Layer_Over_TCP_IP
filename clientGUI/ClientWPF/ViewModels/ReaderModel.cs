using System;

namespace ClientWPF.ViewModels
{
    class ReaderModel
    {
        private readonly int _id;
        private readonly string _name;

        public ReaderModel(int id, string name)
        {
            _id = id;
            _name = name;
        }

        public int ReaderID
        {
            get => _id;
        }

        public string ReaderName
        {
            get => _name;
        }
    }
}
