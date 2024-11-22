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
using System.Windows.Media.Imaging;

namespace BlackJack
{
    public class ObjGui
    {
        public class TypeSkin
        {
            public BitmapImage imgSettings;
            public BitmapImage imgGameBackground;
            public BitmapImage imgResume;
            public BitmapImage imgMainBackground;
            public BitmapImage imgBaseCircle;
            public BitmapImage imgSettingsBackground;
            public BitmapImage imgCards;
            public BitmapImage imgRedChips;
            public BitmapImage imgBlueChips;
            public BitmapImage imgGreenChips;
            public BitmapImage imgBlackChips;
            public BitmapImage imgCardFlip;
            public BitmapImage imgTabWin;
            public BitmapImage imgTabLose;
            public BitmapImage imgTabPush;
            public BitmapImage imgPrev;
            public BitmapImage imgAboutBg;
            public BitmapImage imgAchieveWooh;
            public BitmapImage imgAchieveFire;
            public BitmapImage imgAchieveSplit;
            public BitmapImage imgAchieve25k;
            public BitmapImage imgAchieve50k;
            public BitmapImage imgAchieve75g;
            public BitmapImage imgAchieve100k;
            public BitmapImage imgAchieveLucky;
            public BitmapImage imgAchieveTrip;
            public BitmapImage imgAchieveRunning;
            public BitmapImage imgAchieveHr;
            public BitmapImage imgAchieve8s;
            public BitmapImage imgAchieveGam;
            public BitmapImage imgAchieveDouble;
            public BitmapImage imgAchieveLocked;
            public BitmapImage imgPnts5;
            public BitmapImage imgPnts5Grey;
            public BitmapImage imgPnts10;
            public BitmapImage imgPnts10Grey;
            public BitmapImage imgPnts15;
            public BitmapImage imgPnts15Grey;
            public BitmapImage imgPnts20;
            public BitmapImage imgPnts20Grey;
            public BitmapImage imgPnts25;
            public BitmapImage imgPnts25Grey;

            public bool Load()
            {
                imgSettings = new BitmapImage(new Uri(@"../Assets/appbar.feature.settings.rest.png", UriKind.Relative));
                imgResume = new BitmapImage(new Uri(@"../Assets/appbar.next.rest.png", UriKind.Relative));
                imgBaseCircle = new BitmapImage(new Uri(@"../Assets/appbar.basecircle.rest.png", UriKind.Relative));
                imgPrev = new BitmapImage(new Uri(@"../Assets/appbar.prev.rest.png", UriKind.Relative));

                imgCardFlip = new BitmapImage(new Uri(@"../Assets/cardflip.png", UriKind.Relative));
                imgCards = new BitmapImage(new Uri(@"../Assets/cards.png", UriKind.Relative));

                
                imgGameBackground = new BitmapImage(new Uri(@"Assets/Table.jpg", UriKind.Relative));
                
                imgMainBackground = new BitmapImage(new Uri(@"Assets/Splash.jpg", UriKind.Relative));
                
                imgSettingsBackground = new BitmapImage(new Uri(@"Assets/menu.jpg", UriKind.Relative));

                imgRedChips = new BitmapImage(new Uri(@"../Assets/chips_red.png", UriKind.Relative));
                imgBlueChips = new BitmapImage(new Uri(@"../Assets/chips_blue.png", UriKind.Relative));
                imgGreenChips = new BitmapImage(new Uri(@"../Assets/chips_green.png", UriKind.Relative));
                imgBlackChips = new BitmapImage(new Uri(@"../Assets/chips_black.png", UriKind.Relative));
                
                imgTabWin = new BitmapImage(new Uri(@"Assets/tab_win.png", UriKind.Relative));
                imgTabLose = new BitmapImage(new Uri(@"Assets/tab_lose.png", UriKind.Relative));
                imgTabPush = new BitmapImage(new Uri(@"Assets/tab_push.png", UriKind.Relative));
                
                imgAboutBg = new BitmapImage(new Uri(@"Assets/about.png", UriKind.RelativeOrAbsolute));
                imgAchieveWooh = new BitmapImage(new Uri(@"Assets/achieve_woh.png", UriKind.Relative));
                imgAchieveFire = new BitmapImage(new Uri(@"Assets/achieve_fire.png", UriKind.Relative));
                imgAchieveSplit = new BitmapImage(new Uri(@"Assets/achieve_split.png", UriKind.Relative));
                imgAchieve25k = new BitmapImage(new Uri(@"Assets/achieve_25k.png", UriKind.Relative));
                imgAchieve50k = new BitmapImage(new Uri(@"Assets/achieve_dollar.png", UriKind.Relative));
                imgAchieve75g = new BitmapImage(new Uri(@"Assets/achieve_75g.png", UriKind.Relative));
                imgAchieve100k = new BitmapImage(new Uri(@"Assets/achieve_100.png", UriKind.Relative));
                imgAchieveLucky = new BitmapImage(new Uri(@"Assets/achieve_luck.png", UriKind.Relative));
                imgAchieveTrip = new BitmapImage(new Uri(@"Assets/achieve_bust.png", UriKind.Relative));
                imgAchieveRunning = new BitmapImage(new Uri(@"Assets/achieve_count.png", UriKind.Relative));
                imgAchieveHr = new BitmapImage(new Uri(@"Assets/achieve_hr.png", UriKind.Relative));
                imgAchieve8s = new BitmapImage(new Uri(@"Assets/achieve_8s.png", UriKind.Relative));
                imgAchieveGam = new BitmapImage(new Uri(@"Assets/achieve_gam.png", UriKind.Relative));
                imgAchieveDouble = new BitmapImage(new Uri(@"Assets/achieve_x2.png", UriKind.Relative));
                imgAchieveLocked = new BitmapImage(new Uri(@"Assets/achieve_locked.png", UriKind.Relative));
                imgPnts5 = new BitmapImage(new Uri(@"Assets/pts_5.png", UriKind.Relative));
                imgPnts5Grey = new BitmapImage(new Uri(@"Assets/pts_5_grey.png", UriKind.Relative));
                imgPnts10 = new BitmapImage(new Uri(@"Assets/pts_10.png", UriKind.Relative));
                imgPnts10Grey = new BitmapImage(new Uri(@"Assets/pts_10_grey.png", UriKind.Relative));
                imgPnts15 = new BitmapImage(new Uri(@"Assets/pts_15.png", UriKind.Relative));
                imgPnts15Grey = new BitmapImage(new Uri(@"Assets/pts_15_grey.png", UriKind.Relative));
                imgPnts20 = new BitmapImage(new Uri(@"Assets/pts_20.png", UriKind.Relative));
                imgPnts20Grey = new BitmapImage(new Uri(@"Assets/pts_20_grey.png", UriKind.Relative));
                imgPnts25 = new BitmapImage(new Uri(@"Assets/pts_25.png", UriKind.Relative));
                imgPnts25Grey = new BitmapImage(new Uri(@"Assets/pts_25_grey.png", UriKind.Relative));
                                
                return true;
            }

        }

        public TypeSkin Skin { get; protected set; }

        public ObjGui()
        {
            Skin = new TypeSkin();

            Skin.Load();
        }

        //currently only used on the background and I don't want to scale it - Adrian
        public ImageBrush GetImageBrush(ref BitmapImage img)
        {
            ImageBrush i = new ImageBrush();
            i.Stretch = Stretch.Uniform;
            i.ImageSource = img;

            return i;
        }
    }
}
