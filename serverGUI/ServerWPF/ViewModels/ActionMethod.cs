using System;

namespace ServerWPF.ViewModels
{
    public sealed class ActionMethod
    {
        private readonly String name;
        private readonly int value;

        public static readonly ActionMethod COMMAND = new ActionMethod(1, nameof(COMMAND));
        public static readonly ActionMethod SEND_TYPE_A = new ActionMethod(2, nameof(SEND_TYPE_A));
        public static readonly ActionMethod SEND_TYPE_B = new ActionMethod(3, nameof(SEND_TYPE_B));
        public static readonly ActionMethod SEND_TYPE_F = new ActionMethod(4, nameof(SEND_TYPE_F));
        public static readonly ActionMethod ECHO = new ActionMethod(5, nameof(ECHO));
        public static readonly ActionMethod DIAG = new ActionMethod(6, nameof(DIAG));
        public static readonly ActionMethod COLD_RESET = new ActionMethod(7, nameof(COLD_RESET));
        public static readonly ActionMethod WARM_RESET = new ActionMethod(8, nameof(WARM_RESET));
        public static readonly ActionMethod POWER_OFF_FIELD = new ActionMethod(8, nameof(POWER_OFF_FIELD));
        public static readonly ActionMethod POWER_ON_FIELD = new ActionMethod(9, nameof(POWER_ON_FIELD));
        public static readonly ActionMethod RESTART = new ActionMethod(10, nameof(RESTART));
        public static readonly ActionMethod STOP_CLIENT = new ActionMethod(11, nameof(STOP_CLIENT));
        public static readonly ActionMethod GET_VERSION = new ActionMethod(12, nameof(GET_VERSION));
        public static readonly ActionMethod POLL_TYPE_A = new ActionMethod(13, nameof(POLL_TYPE_A));
        public static readonly ActionMethod POLL_TYPE_B = new ActionMethod(14, nameof(POLL_TYPE_B));
        public static readonly ActionMethod POLL_TYPE_F = new ActionMethod(15, nameof(POLL_TYPE_F));

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
