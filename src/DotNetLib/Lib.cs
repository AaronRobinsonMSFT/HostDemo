using System;

namespace DotNetLib
{
    public class Lib
    {
        private static int s_CallCount = 1;
        public static int Hello(IntPtr arg, int argLength)
        {
            Console.WriteLine($"Hello, world! from {nameof(Lib)} [count: {s_CallCount++}]");
            return 0;
        }
    }
}
