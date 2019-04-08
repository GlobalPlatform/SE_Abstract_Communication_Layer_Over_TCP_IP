using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerWPF.ViewModels
{
    public sealed class ActionMethod
    {
        private readonly String name;
        private readonly int value;

        public static readonly ActionMethod COMMAND = new ActionMethod(1, "COMMAND");
        public static readonly ActionMethod SEND_TYPE_A = new ActionMethod(2, "SEND TYPE A");
        public static readonly ActionMethod SEND_TYPE_B = new ActionMethod(3, "SEND TYPE B");
        public static readonly ActionMethod SEND_TYPE_F = new ActionMethod(4, "SEND TYPE F");
        public static readonly ActionMethod ECHO = new ActionMethod(5, "ECHO");
        public static readonly ActionMethod DIAG = new ActionMethod(6, "DIAG");
        public static readonly ActionMethod COLD_RESET = new ActionMethod(7, "COLD RESET");
        public static readonly ActionMethod WARM_RESET = new ActionMethod(8, "WARM RESET");
        public static readonly ActionMethod POWER_OFF_FIELD = new ActionMethod(8, "POWER OFF FIELD");
        public static readonly ActionMethod POWER_ON_FIELD = new ActionMethod(9, "POWER ON FIELD");
        public static readonly ActionMethod RESTART = new ActionMethod(10, "RESTART");
        public static readonly ActionMethod STOP_CLIENT = new ActionMethod(11, "STOP CLIENT");

        private ActionMethod(int value, String name)
        {
            this.name = name;
            this.value = value;
        }

        public override String ToString()
        {
            return name;
        }
    }
}
