using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace EV3DataListener
{
    struct Element
    {
        public string Name;
        public float Value;
    }


    class Program
    {
        static void Main(string[] args)
        {
            const int refreshRateMs = 50;
            int packetCount = 0;
            int updateCount = 0;
            bool isDirty = true;
            Element[] data = new Element[256];
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
                        Array.Clear(data, 0, data.Length);
                        Console.Clear();
                    }
                    else if (buffer.Length == 5)
                    {
                        float newValue = BitConverter.ToSingle(buffer, 0);
                        int index = buffer[4];
                        if (data[index].Value != newValue)
                        {
                            data[index].Value = newValue;
                            isDirty = true;
                        }
                    }
                    else if (buffer.Length >= 7)
                    {
                        float newValue = BitConverter.ToSingle(buffer, 0);
                        int index = buffer[5];
                        int length = buffer[6];

                        data[index].Name = Encoding.UTF8.GetString(buffer, 7, length);
                        if (data[index].Value != newValue)
                        {
                            data[index].Value = newValue;
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
                        foreach (var pair in data.Where(s => s.Name != null).OrderBy(s => s.Name))
                        {
                            text.AppendLine($"{pair.Name,16}: {pair.Value,-16}               ");
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
