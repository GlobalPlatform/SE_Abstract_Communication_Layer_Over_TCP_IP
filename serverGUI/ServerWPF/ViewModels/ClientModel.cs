namespace ServerWPF.ViewModels
{
    public class ClientModel
    {
        private int _id;
        private string _name;

        public ClientModel(int id, string name)
        {
            _id = id;
            _name = name;
        }

        public int ClientID
        {
            get => _id; 
            set => _id = value; 
        }

        public string ClientName
        {
            get =>_name; 
            set => _name = value; 
        }
    }
}