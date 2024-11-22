using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;

namespace PitchPerfect.Controls
{
    /// <summary>
    /// Interaction logic for PianoControlWPF.xaml
    /// </summary>
    public partial class PianoControlWPF : UserControl
    {
        private const int LowNoteID = 24;//21;

        private const int HighNoteID = LowNoteID+48;//109;

        int iKeyWidth = 16;//8
        int iKeyHeight = 96;//48

        int ibKeyWidth = 10;//5
        int ibKeyHeight = 62; //31

        int ibOffSet = -6;//-3

        //private delegate void NoteMessageCallback(ChannelMessage message);

      //  private NoteMessageCallback noteOnCallback;

      //  private NoteMessageCallback noteOffCallback;

        SynchronizationContext context;

        List<PianoKeyWPF> keys = new List<PianoKeyWPF>();

        int whiteKeyCount = 0;

        public enum KeyType
        {
            White,
            Black
        }

        private static readonly KeyType[] KeyTypeTable = 
            {
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White,
                KeyType.White, KeyType.Black, KeyType.White, KeyType.Black, KeyType.White, KeyType.White, KeyType.Black, KeyType.White
            };

        private static readonly int[] Ids = 
            {
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
                0, 1, 0, 1, 0, 0, 1, 0
            };

        public PianoControlWPF()
        {
            InitializeComponent();

            CreatePianoKeys();

            //InitializePianoKeys();

            context = SynchronizationContext.Current;

            /*noteOnCallback = delegate(ChannelMessage message)
            {
                if (message.Data2 > 0)
                {
                    keys[message.Data1 - LowNoteID].PressPianoKey();
                }
                else
                {
                    keys[message.Data1 - LowNoteID].ReleasePianoKey();
                }
            };

            noteOffCallback = delegate(ChannelMessage message)
            {
                keys[message.Data1 - LowNoteID].ReleasePianoKey();
            };*/
        }

        private void CreatePianoKeys()
        {
            whiteKeyCount = 0;
            double nextLeft = 0;
            for (int i = 0; i < HighNoteID - LowNoteID; i++)
            {
                var key = new PianoKeyWPF(KeyTypeTable[i + LowNoteID])
                {
                };

                key.NoteID = i + LowNoteID;

                if (KeyTypeTable[key.NoteID] == KeyType.White)
                {
                    key.NoteOffColor = Colors.White;

                    whiteKeyCount++;
                    key.Width = iKeyWidth;
                    key.Height = iKeyHeight;

                    key.Margin = new Thickness(nextLeft, 0, 0, 0);
                    nextLeft += iKeyWidth;
                    key.SetValue(Canvas.ZIndexProperty, 0);
                }
                else
                {
                    key.NoteOffColor = Colors.Black;

                    key.Width = ibKeyWidth;
                    key.Height = ibKeyHeight;
                    key.Margin = new Thickness(nextLeft + ibOffSet, 0, 0, 0);
                    key.SetValue(Canvas.ZIndexProperty, 10);
                }

                keys.Add(key);
                cnvPiano.Children.Add(key);
            }
        }

    /*    public void Send(ChannelMessage message)
        {
            if (message.Command == ChannelCommand.NoteOn &&
                message.Data1 >= LowNoteID && message.Data1 <= HighNoteID)
            {
                noteOnCallback(message);
            }
            else if (message.Command == ChannelCommand.NoteOff &&
                message.Data1 >= LowNoteID && message.Data1 <= HighNoteID)
            {
                noteOffCallback(message);
            }
        }*/

        public void Clear()
        {
            for (var i = LowNoteID; i < HighNoteID; i++)
            {
                keys[i - LowNoteID].ReleasePianoKey();
            }
        }
    }
}
