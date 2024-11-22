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
using System.Resources;
using System.Globalization;
using System.Reflection;
using System.Threading;
using System.ComponentModel;

namespace GuitarSuite
{
    public class LocalizedStrings
    {
        public LocalizedStrings()
        {
        }

        private static GuitarSuite.Strings.StringTable _localizedresources = new GuitarSuite.Strings.StringTable();

        public GuitarSuite.Strings.StringTable Localizedresources { get { return _localizedresources; } }
    
    }
}
