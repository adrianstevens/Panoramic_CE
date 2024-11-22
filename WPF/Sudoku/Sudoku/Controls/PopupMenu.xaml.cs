using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Windows.Navigation;


namespace Sudoku
{
    public enum EnumPopupItem
    {
        Single,
        Double,
        Image,
        Separator
    }
    
    public partial class PopupMenu : UserControl
    {
        // location properties
        private double _dbOriginX;
        private double _dbOriginY;
        private double _dbDestinationX;
        private double _dbDestinationY;
        private Grid _gridParentLayout;
        private DateTime _dtPopupTime;
        private FrameworkElement _ctrlItem;
        private int _iLastSelectedItem = -1;
        string szTitle = "";

        public static bool bPopupActivated = false;
        public static PopupMenu popupCurrent;
        public static ObservableCollection<PopupMenuItems> _arrItems;

        public EventHandler<PopupMenuEventArgs> SelectionChanged;

        public PopupMenu(FrameworkElement ctrlItem, string title)
        {
            InitializeComponent();

            _arrItems = new ObservableCollection<PopupMenuItems>();

            this.DataContext = _arrItems;

            szTitle = title;
            popupCurrent = this;
            bPopupActivated = false;

            _ctrlItem = ctrlItem;            
        }

        public void AddItem(string item)
        {
            PopupMenuItems newContent = new PopupMenuItems();
            newContent.szItem = item;
            newContent.eItem = EnumPopupItem.Single;
            _arrItems.Add(newContent);
        }
        public void AddItem(string item, string subitem)
        {
            PopupMenuItems newContent = new PopupMenuItems();
            newContent.szItem = item;
            newContent.szSubItem = subitem;
            newContent.eItem = EnumPopupItem.Double;
            _arrItems.Add(newContent);
        }
        public void AddImageItem(string item, string szSource)
        {
            PopupMenuItems newContent = new PopupMenuItems();
            newContent.szItem = item;
            newContent.szSubItem = szSource;
            newContent.eItem = EnumPopupItem.Image;
            _arrItems.Add(newContent);
        }
        public void AddSeparator()
        {
            PopupMenuItems newContent = new PopupMenuItems();
            newContent.eItem = EnumPopupItem.Separator;
            _arrItems.Add(newContent);
        }

        public void SetCurrentIndex(int iIndex)
        {
            if (iIndex >= myListBox.Items.Count)
                return;

            // have to save the time
            _dtPopupTime = DateTime.Now;

            myListBox.SelectedIndex = _iLastSelectedItem = iIndex;
        }

        public void Launch()
        {
            

            // get the parent page we need
            UIElement el = _ctrlItem as UIElement;
            
            if(!(el is Page))
            {
                el = VisualTreeHelper.GetParent(_ctrlItem) as UIElement;
                while (!(el is Page))
                {
                    el = VisualTreeHelper.GetParent(el) as UIElement;
                }
            }
            _gridParentLayout = VisualTreeHelper.GetChild(el, 0) as Grid;

            // go full screen if we have enough items
            if (_arrItems.Count >= 8)
            {
                Page p = (Page)el;
                p.NavigationService.Navigate(new Uri("/Controls/PopupMenuFull.xaml?index="+_iLastSelectedItem.ToString()+"&title="+szTitle, UriKind.Relative));
                return;
            }


            GeneralTransform gt = _ctrlItem.TransformToVisual(el);
            Point offset = gt.Transform(new Point(0, 0));


            // put up the popup menu
            _dbOriginX = offset.X + (_ctrlItem.ActualWidth / 2);  // middle postion: + width/2
            _dbOriginY = offset.Y + (_ctrlItem.ActualHeight / 2);  // middle postion: + height/2

            // get destination: middle of the app

            double dbDestinationX = _gridParentLayout.ActualWidth / 2;
            double dbDestinationY = _gridParentLayout.ActualHeight / 2;

            int iMaxHeight = (int)_gridParentLayout.ActualHeight - 2*100;

            int iListBoxHeight = 0;
            for (int i = 0; i < myListBox.Items.Count; i++)
            {
                iListBoxHeight += _arrItems[i].iItemHeight;
            }

            iListBoxHeight += 20;
            iListBoxHeight = (iListBoxHeight>iMaxHeight?iMaxHeight:iListBoxHeight);


            gridMain.Height = iListBoxHeight + 20;

            // this is where the LEFT-BOTTOM X-Y-coördinates will
            // end up - so we adjust these to our liking; in this case,
            // I change 'em so the controls "pops up" in the middle of its parent
            // by adjusting the destination-coördinates by half of its own width/height

            _dbDestinationX = dbDestinationX - (gridMain.Width / 2);
            _dbDestinationY = dbDestinationY - (iListBoxHeight / 2);

            //_dbDestinationY = 0;

            this.SetValue(Grid.RowSpanProperty, 3);
            this.SetValue(Grid.ColumnSpanProperty, 3);

            bPopupActivated = true;

            // create the correct animation, begin the animation
            CreateTranslateAndScaleIn(gridMain, _dbOriginX, _dbOriginY, _dbDestinationX, _dbDestinationY).Begin();           

            // have to save the time
            _dtPopupTime = DateTime.Now;

            _gridParentLayout.Children.Add(this);

            Action myScroll = delegate
            {
                myListBox.ScrollIntoView(myListBox.SelectedItem);
            };

            Dispatcher.BeginInvoke(myScroll);
            
        }
        public static Storyboard CreateTranslateAndScaleIn(DependencyObject objectToBeAnimated, double fromX, double fromY, double toX, double toY)
        {
            Storyboard sb = new Storyboard();

            DoubleAnimationUsingKeyFrames animScaleX = new DoubleAnimationUsingKeyFrames();
            DoubleAnimationUsingKeyFrames animScaleY = new DoubleAnimationUsingKeyFrames();
            DoubleAnimationUsingKeyFrames animTranslateX = new DoubleAnimationUsingKeyFrames();
            DoubleAnimationUsingKeyFrames animTranslateY = new DoubleAnimationUsingKeyFrames();
            DoubleAnimationUsingKeyFrames animOpacity = new DoubleAnimationUsingKeyFrames();


            // scaleX animation

            animScaleX.BeginTime = new TimeSpan(0);
            animScaleX.KeyFrames.Add(getSplineDoubleKeyFrame(0, 0));
            animScaleX.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, 1));
            sb.Children.Add(animScaleX);

            Storyboard.SetTarget(animScaleX, objectToBeAnimated);
            Storyboard.SetTargetProperty(animScaleX, new PropertyPath("(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.ScaleX)"));


            // scaleY animation

            animScaleY.BeginTime = new TimeSpan(0);
            animScaleY.KeyFrames.Add(getSplineDoubleKeyFrame(0, 0));
            animScaleY.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, 1));
            sb.Children.Add(animScaleY);

            Storyboard.SetTarget(animScaleY, objectToBeAnimated);
            Storyboard.SetTargetProperty(animScaleY, new PropertyPath("(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.ScaleY)"));


            animTranslateX.BeginTime = new TimeSpan(0);
            animTranslateX.KeyFrames.Add(getSplineDoubleKeyFrame(0, fromX));
            animTranslateX.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, toX));
            sb.Children.Add(animTranslateX);

            Storyboard.SetTarget(animTranslateX, objectToBeAnimated);
            Storyboard.SetTargetProperty(animTranslateX, new PropertyPath("(UIElement.RenderTransform).(TransformGroup.Children)[3].(TranslateTransform.X)"));


            animTranslateY.BeginTime = new TimeSpan(0);
            animTranslateY.KeyFrames.Add(getSplineDoubleKeyFrame(0, fromY));
            animTranslateY.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, toY));
            sb.Children.Add(animTranslateY);

            Storyboard.SetTarget(animTranslateY, objectToBeAnimated);
            Storyboard.SetTargetProperty(animTranslateY, new PropertyPath("(UIElement.RenderTransform).(TransformGroup.Children)[3].(TranslateTransform.Y)"));

            animOpacity.BeginTime = new TimeSpan(0);
            animOpacity.KeyFrames.Add(getSplineDoubleKeyFrame(0, 0));
            animOpacity.KeyFrames.Add(getSplineDoubleKeyFrame(0.1, 1));
            sb.Children.Add(animOpacity);

            Storyboard.SetTarget(animOpacity, objectToBeAnimated);
            Storyboard.SetTargetProperty(animOpacity, new PropertyPath("(UIElement.Opacity)"));


            return sb;
        }
        public static Storyboard CreateTranslateAndScaleOut(DependencyObject objectToBeAnimated, double fromX, double fromY, double toX, double toY)
        {
            Storyboard sb = new Storyboard();

            DoubleAnimationUsingKeyFrames animScaleX = new DoubleAnimationUsingKeyFrames();
            DoubleAnimationUsingKeyFrames animScaleY = new DoubleAnimationUsingKeyFrames();
            DoubleAnimationUsingKeyFrames animTranslateX = new DoubleAnimationUsingKeyFrames();
            DoubleAnimationUsingKeyFrames animTranslateY = new DoubleAnimationUsingKeyFrames();
            DoubleAnimationUsingKeyFrames animOpacity = new DoubleAnimationUsingKeyFrames();

            // scaleX animation

            animScaleX.BeginTime = new TimeSpan(0);
            animScaleX.KeyFrames.Add(getSplineDoubleKeyFrame(0, 1));
            animScaleX.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, 0));
            sb.Children.Add(animScaleX);

            Storyboard.SetTarget(animScaleX, objectToBeAnimated);
            Storyboard.SetTargetProperty(animScaleX, new PropertyPath("(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.ScaleX)"));


            // scaleY animation

            animScaleY.BeginTime = new TimeSpan(0);
            animScaleY.KeyFrames.Add(getSplineDoubleKeyFrame(0, 1));
            animScaleY.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, 0));
            sb.Children.Add(animScaleY);

            Storyboard.SetTarget(animScaleY, objectToBeAnimated);
            Storyboard.SetTargetProperty(animScaleY, new PropertyPath("(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.ScaleY)"));


            animTranslateX.BeginTime = new TimeSpan(0);
            animTranslateX.KeyFrames.Add(getSplineDoubleKeyFrame(0, toX));
            animTranslateX.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, fromX));
            sb.Children.Add(animTranslateX);

            Storyboard.SetTarget(animTranslateX, objectToBeAnimated);
            Storyboard.SetTargetProperty(animTranslateX, new PropertyPath("(UIElement.RenderTransform).(TransformGroup.Children)[3].(TranslateTransform.X)"));


            animTranslateY.BeginTime = new TimeSpan(0);
            animTranslateY.KeyFrames.Add(getSplineDoubleKeyFrame(0, toY));
            animTranslateY.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, fromY));
            sb.Children.Add(animTranslateY);

            Storyboard.SetTarget(animTranslateY, objectToBeAnimated);
            Storyboard.SetTargetProperty(animTranslateY, new PropertyPath("(UIElement.RenderTransform).(TransformGroup.Children)[3].(TranslateTransform.Y)"));

            animOpacity.BeginTime = new TimeSpan(0);
            animOpacity.KeyFrames.Add(getSplineDoubleKeyFrame(0.1, 1));
            animOpacity.KeyFrames.Add(getSplineDoubleKeyFrame(0.2, 0));
            sb.Children.Add(animOpacity);

            Storyboard.SetTarget(animOpacity, objectToBeAnimated);
            Storyboard.SetTargetProperty(animOpacity, new PropertyPath("(UIElement.Opacity)"));


            return sb;
        }

        private void Background_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            CloseWindow();
        }
        private void myListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (myListBox.SelectedIndex != _iLastSelectedItem && _arrItems[myListBox.SelectedIndex].IsEnabled == false)
            {                
                // have to save the time
                _dtPopupTime = DateTime.Now;
                // set the old selection
                myListBox.SelectedIndex = _iLastSelectedItem;
                return;
            }

            // this is so we don't close the window when selecting the default selector
            if (DateTime.Now.Subtract(_dtPopupTime).TotalMilliseconds < 150)//1000 is WAY too long ...
            {
                return;
            }

            if (SelectionChanged != null)
            {
                this.SelectionChanged(sender, new PopupMenuEventArgs
                {
                    iSelected = myListBox.SelectedIndex
                });
            }

            CloseWindow();
        }
        /// <summary>
        /// Helper method - create SplineDoubleKeyFrame, based on seconds for duration & value to animate to
        /// </summary>
        /// <param name="seconds"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        private static SplineDoubleKeyFrame getSplineDoubleKeyFrame(double seconds, double value)
        {
            return new SplineDoubleKeyFrame()
            {
                KeyTime = KeyTime.FromTimeSpan(TimeSpan.FromSeconds(seconds))
               ,
                Value = value
            };
        }
        public void CloseWindow()
        {
            bPopupActivated = false;

            // complete animation, "pop me in"
            Storyboard sb = CreateTranslateAndScaleOut(gridMain, _dbOriginX, _dbOriginY, _dbDestinationX, _dbDestinationY);
            sb.Completed += new EventHandler(sb_Completed);
            sb.Begin();
        }
        private void sb_Completed(object sender, EventArgs e)
        {
            // remove control =>
            if (this.Parent != null)
            {
                ((Grid)this.Parent).Children.Remove(this);
            }
        }
        private void Canvas_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            CloseWindow();
        }
    }

    public class PopupMenuEventArgs : EventArgs
    {
        public int iSelected { get; set; }
    }

    public class PopupMenuItems : INotifyPropertyChanged
    {
        public string szItem { get; set; }
        public string szSubItem { get; set; }
        public EnumPopupItem eItem { get; set; }
        public Visibility visSingle
        {
            get
            {
                switch(eItem)
                {
                    case EnumPopupItem.Single:
                        return Visibility.Visible;
                     default:
                        return Visibility.Collapsed;
                }
            }
        }
        public Visibility visDouble
        {
            get
            {
                switch (eItem)
                {
                    case EnumPopupItem.Double:
                        return Visibility.Visible;
                    default:
                        return Visibility.Collapsed;
                }
            }
        }
        public Visibility visSeparator
        {
            get
            {
                switch (eItem)
                {
                    case EnumPopupItem.Separator:
                        return Visibility.Visible;
                    default:
                        return Visibility.Collapsed;
                }
            }
        }
        public Visibility visImage
        {
            get
            {
                switch (eItem)
                {
                    case EnumPopupItem.Image:
                        return Visibility.Visible;
                    default:
                        return Visibility.Collapsed;
                }
            }
        }
        public bool IsEnabled
        {            
            get
            {
                switch (eItem)
                {
                    case EnumPopupItem.Separator:
                        return false;
                    default:
                        return true;
                }
            }
        }
        public int iItemHeight
        {
            get
            {
                switch (eItem)
                {
                    case EnumPopupItem.Single:
                        return 76;
                    case EnumPopupItem.Double:
                        return 100;
                    default:
                        return 100;
                }
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;
    }
}
