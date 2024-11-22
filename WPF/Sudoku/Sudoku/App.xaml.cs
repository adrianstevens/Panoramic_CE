using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;

namespace Sudoku
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private void Application_SessionEnding(object sender, SessionEndingCancelEventArgs e)
        {
            // save all the values
            Globals.Settings.Save();

            if (Globals.Settings.bIsGameInPlay)
                Globals.Game.SaveGameState();
        }

        private void Application_Exit(object sender, ExitEventArgs e)
        {
            // save all the values
            Globals.Settings.Save();

            if (Globals.Settings.bIsGameInPlay)
                Globals.Game.SaveGameState();
        }
    }
}
