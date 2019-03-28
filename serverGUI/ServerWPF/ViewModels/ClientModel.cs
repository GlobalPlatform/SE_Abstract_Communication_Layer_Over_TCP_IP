namespace ServerWPF.ViewModels
{
    public class ClientModel
    {
        private int _id;
        private string _name;

        public ClientModel(int id, string name)
        {
            this._id = id;
            this._name = name;
        }

        public int ClientID
        {
            get { return _id; }
            set { _id = value; }
        }

        public string ClientName
        {
            get { return _name; }
            set { _name = value; }
        }
    }
}