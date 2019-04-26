using System;
using System.Collections.Generic;

namespace ServerWPF.ViewModels
{
    public class ActionFlyweight
    {
        private Dictionary<string, Func<bool>> flyweightActions = new Dictionary<string, Func<bool>>();

        public void AddAction(string key, Func<bool> value)
        {
            flyweightActions.Add(key.ToString(), value);
        }

        public Func<bool> GetAction(string key)
        {
            return flyweightActions.ContainsKey(key) ? flyweightActions[key] : null;
        }
    }
}
