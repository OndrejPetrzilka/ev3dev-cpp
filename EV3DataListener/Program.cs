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
                        // Empty packet, clear all data
                        Array.Clear(data, 0, data.Length);
                        Console.Clear();
                    }
                    else
                    {
                        ReadBuffer(buffer, data, ref isDirty);
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

        private static void ReadBuffer(byte[] buffer, Element[] data, ref bool isDirty)
        {
            int pos = 0;
            while (buffer.Length - pos >= 5)
            {
                float newValue = BitConverter.ToSingle(buffer, pos);
                pos += 4;
                int index = buffer[pos++];
                if (index == 255)
                {
                    // Value with description
                    if (buffer.Length - pos < 2)
                        return;

                    index = buffer[pos++];
                    int length = buffer[pos++];

                    if (buffer.Length - pos < length)
                        return;

                    data[index].Name = Encoding.UTF8.GetString(buffer, pos, length);
                    pos += length;
                    if (data[index].Value != newValue)
                    {
                        data[index].Value = newValue;
                        isDirty = true;
                    }
                }
                else if (data[index].Value != newValue)
                {
                    // Value with just index
                    if (data[index].Name == null)
                    {
                        data[index].Name = $"[{index,3}]";
                    }
                    data[index].Value = newValue;
                    isDirty = true;
                }
            }
        }
    }
}
