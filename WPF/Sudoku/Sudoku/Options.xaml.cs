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

namespace Sudoku
{
    /// <summary>
    /// Interaction logic for Options.xaml
    /// </summary>
    public partial class Options : Page
    {
        public Options()
        {
            InitializeComponent();

            string szAbout = Globals.rm.GetString("IDS_ABOUT_1");
            txtAboutCopyright.Text = szAbout.Replace("\\r\\n", "\r\n");

            // skins
            optSkin.txtTitle = Globals.rm.GetString("IDS_PREF_Skin");
          /*  optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Glass"), "/Assets/Options/skin_glass.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Wood"), "/Assets/Options/skin_wood.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Notepad"), "/Assets/Options/skin_notepad.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Grey"), "/Assets/Options/skin_grey.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Cust1"), "/Assets/Options/skin_custom.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Cust2"), "/Assets/Options/skin_custom1.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Cust3"), "/Assets/Options/skin_custom2.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Cust4"), "/Assets/Options/skin_custom3.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Cust5"), "/Assets/Options/skin_custom4.jpg");
            optSkin.AddImageItem(Globals.rm.GetString("IDS_PREF_Cust6"), "/Assets/Options/skin_custom5.jpg");*/

            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Glass"));
            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Wood"));
         //   optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Notepad"));
            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Grey") );
            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Cust1"));
            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Cust2"));
            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Cust3") );
            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Cust4") );
            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Cust5")); 
            optSkin.AddItem(Globals.rm.GetString("IDS_PREF_Cust6")); 
            
            optSkin.SetCurrentIndex((int)Globals.Settings.eSkin);
            optSkin.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.eSkin = (EnumSudokuSkins)iSelIndex;
            };

            // Play Sounds
            optPlaySounds.txtTitle = Globals.rm.GetString("IDS_PREF_PlaySounds");
            optPlaySounds.AddItem(Globals.rm.GetString("IDS_MENU_No"));
            optPlaySounds.AddItem(Globals.rm.GetString("IDS_MENU_Yes"));
            optPlaySounds.SetCurrentIndex(Globals.Settings.bPlaySounds ? 1 : 0);
            optPlaySounds.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bPlaySounds = iSelIndex == 0 ? false : true;
            };

            // Show Errors
            optShowErrors.txtTitle = Globals.rm.GetString("IDS_PREF_ShowErrors");
            optShowErrors.AddItem(Globals.rm.GetString("IDS_MENU_No"));
            optShowErrors.AddItem(Globals.rm.GetString("IDS_MENU_Yes"));
            optShowErrors.SetCurrentIndex(Globals.Settings.bShowErrors ? 1 : 0);
            optShowErrors.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bShowErrors = iSelIndex == 0 ? false : true;
            };

            // Lock Completed
            optLockCompleted.txtTitle = Globals.rm.GetString("IDS_PREF_LockCompleted");
            optLockCompleted.AddItem(Globals.rm.GetString("IDS_MENU_No"));
            optLockCompleted.AddItem(Globals.rm.GetString("IDS_MENU_Yes"));
            optLockCompleted.SetCurrentIndex(Globals.Settings.bLockCompleted ? 1 : 0);
            optLockCompleted.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bLockCompleted = iSelIndex == 0 ? false : true;
            };

            // Show Hints
            optShowHints.txtTitle = Globals.rm.GetString("IDS_PREF_ShowHints");
            optShowHints.AddItem(Globals.rm.GetString("IDS_MENU_No"));
            optShowHints.AddItem(Globals.rm.GetString("IDS_MENU_Yes"));
            optShowHints.SetCurrentIndex(Globals.Settings.bShowHints ? 1 : 0);
            optShowHints.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bShowHints = iSelIndex == 0 ? false : true;
            };

            // Show Highlights
            optShowHighlights.txtTitle = Globals.rm.GetString("IDS_PREF_ShowHighlights");
            optShowHighlights.AddItem(Globals.rm.GetString("IDS_MENU_No"));
            optShowHighlights.AddItem(Globals.rm.GetString("IDS_MENU_Yes"));
            optShowHighlights.SetCurrentIndex(Globals.Settings.bShowHighlights ? 1 : 0);
            optShowHighlights.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bShowHighlights = iSelIndex == 0 ? false : true;
            };

            // auto pencil marks
            optAutoPencil.txtTitle = Globals.rm.GetString("IDS_MENU_AutoPencil");
            optAutoPencil.AddItem(Globals.rm.GetString("IDS_MENU_No"));
            optAutoPencil.AddItem(Globals.rm.GetString("IDS_MENU_Yes"));
            optAutoPencil.SetCurrentIndex(Globals.Settings.bAutoPencil ? 1 : 0);
            optAutoPencil.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bAutoPencil = iSelIndex == 0 ? false : true;
            };

            // auto remove pencil marks
            optAutoClearPencilMarks.txtTitle = Globals.rm.GetString("IDS_PREF_AutoClear");
            optAutoClearPencilMarks.AddItem(Globals.rm.GetString("IDS_MENU_No"));
            optAutoClearPencilMarks.AddItem(Globals.rm.GetString("IDS_MENU_Yes"));
            optAutoClearPencilMarks.SetCurrentIndex(Globals.Settings.bAutoRemovePencil ? 1 : 0);
            optAutoClearPencilMarks.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bAutoRemovePencil = iSelIndex == 0 ? false : true;
            };

            // draw kakuro blanks
            optDrawBlankCells.txtTitle = Globals.rm.GetString("IDS_PREF_DrawCells");
            optDrawBlankCells.AddItem(Globals.rm.GetString("IDS_MENU_No"));
            optDrawBlankCells.AddItem(Globals.rm.GetString("IDS_MENU_Yes"));
            optDrawBlankCells.SetCurrentIndex(Globals.Settings.bDrawBlanks ? 1 : 0);
            optDrawBlankCells.IndexUpdate += delegate(int iID, int iSelIndex)
            {
                Globals.Settings.bDrawBlanks = iSelIndex == 0 ? false : true;
            };
        }

        private void Grid_Loaded(object sender, RoutedEventArgs e)
        {
           /* ImageBrush i = new ImageBrush();
            i.Stretch = Stretch.UniformToFill;
            i.ImageSource = new BitmapImage(new Uri("../../Assets/OptionsBG.jpg", UriKind.Relative));

            gridMain.Background = i;*/
        }
    }
}
