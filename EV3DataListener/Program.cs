using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace EV3DataListener
{
    class Program
    {

        static void Main(string[] args)
        {
            const int refreshRateMs = 50;
            int packetCount = 0;
            int updateCount = 0;
            bool isDirty = true;
            Dictionary<string, float> data = new Dictionary<string, float>();
            StringBuilder text = new StringBuilder(1024);
            Stopwatch timer = Stopwatch.StartNew();

            Console.CursorVisible = false;

            UdpClient client = new UdpClient(9999, AddressFamily.InterNetwork);
            Task<UdpReceiveResult> task = null;
            while (!Console.KeyAvailable || Console.ReadKey().Key != ConsoleKey.Escape)
            {
                updateCount++;
                if (task == null)
                {
                    task = client.ReceiveAsync();
                }
                if (task.Wait(refreshRateMs))
                {
                    packetCount++;
                    var buffer = task.Result.Buffer;
                    task = null;

                    if (buffer.Length == 0)
                    {
                        data.Clear();
                        Console.Clear();
                    }
                    else if (buffer.Length >= 4)
                    {
                        float newValue = BitConverter.ToSingle(buffer, 0);
                        string key = Encoding.UTF8.GetString(buffer, 4, buffer.Length - 4);
                        if (!data.TryGetValue(key, out float value) || value != newValue)
                        {
                            data[key] = newValue;
                            isDirty = true;
                        }
                    }
                }

                if (timer.Elapsed.TotalMilliseconds > refreshRateMs)
                {
                    float updatesPerSecond = 1000.0f / refreshRateMs;
                    int offset = ((int)Math.Round(updateCount / updatesPerSecond * 2)) % 3 + 1;
                    string dots = new string('.', offset);
                    string footer = $"#{packetCount} {dots}       ";

                    if (isDirty)
                    {
                        Console.SetCursorPosition(0, 0);
                        text.Clear();
                        foreach (var pair in data.OrderBy(s => s.Key))
                        {
                            text.AppendLine($"{pair.Key,16}: {pair.Value,-16}               ");
                        }
                        text.AppendLine("                                 ");
                        text.Append(footer);
                        Console.Write(text.ToString());
                        isDirty = false;
                    }
                    else
                    {
                        Console.SetCursorPosition(0, Console.CursorTop);
                        Console.Write(footer);
                    }
                }
            }
        }
    }
}
