using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace BlackJack.Controls
{
    public partial class BetStack : UserControl
    {
        public BetStack()
        {
            InitializeComponent();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            chipsBlack.SetBackground(Globals.Gui.Skin.imgBlackChips);
            chipsBlue.SetBackground(Globals.Gui.Skin.imgBlueChips);
            chipsRed.SetBackground(Globals.Gui.Skin.imgRedChips);
            chipsGreen.SetBackground(Globals.Gui.Skin.imgGreenChips);

            

        }

        public void CopyChips(BetStack hand)
        {

            chipsGreen.CopyChips(hand.chipsGreen);
            chipsBlack.CopyChips(hand.chipsBlack);
            chipsBlue.CopyChips(hand.chipsBlue);
            chipsRed.CopyChips(hand.chipsRed);
        }


        public void ClearChips()
        {
            chipsRed.Clear();
            chipsGreen.Clear();
            chipsBlue.Clear();
            chipsBlack.Clear();
        }



        public void ResetChips()
        {
            chipsRed.Reset();
            chipsGreen.Reset();
            chipsBlue.Reset();
            chipsBlack.Reset();
        }
    }
}
