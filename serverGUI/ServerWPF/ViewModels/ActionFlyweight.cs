using System;
using System.Collections.Generic;

namespace ServerWPF.ViewModels
{
    public class ActionFlyweight
    {
        private Dictionary<string, Action<object>> flyweightActions = new Dictionary<string, Action<object>>();

        public void addAction(string key, Action<object> value)
        {
            flyweightActions.Add(key.ToString(), value);
        }

        public Action<object> getAction(string key)
        {
            return flyweightActions[key];
        }
    }
}
