using System;
using System.Collections.Generic;

namespace ServerWPF.ViewModels
{
    public class ActionFlyweight
    {
        private Dictionary<string, Action> flyweightActions = new Dictionary<string, Action>();

        public void AddAction(string key, Action value)
        {
            flyweightActions.Add(key.ToString(), value);
        }

        public Action GetAction(string key)
        {
            return flyweightActions.ContainsKey(key) ? flyweightActions[key] : null;
        }
    }
}
