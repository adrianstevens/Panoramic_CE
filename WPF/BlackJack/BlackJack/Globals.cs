using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using BlackJack.Objects;
using System.Resources;
using System.Reflection;

namespace BlackJack
{
    public static class Globals
    {
        public static readonly Random Random = new Random();

        public static ResourceManager Res = new ResourceManager("BlackJack.Strings.StringTable", Assembly.GetExecutingAssembly());

        public static readonly ObjGui Gui = new ObjGui();

        public static readonly BJEngine Game = new BJEngine();

        public static AppSettings Settings = new AppSettings();

        public static void Save()
        {
            Game.Save();
            Settings.Save();
        }
    }
}
