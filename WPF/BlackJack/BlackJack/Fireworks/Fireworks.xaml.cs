
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
using System.Windows.Threading;

using HslColorSample;


namespace Visual
{
    public partial class Fireworks : UserControl
    {
        public double XGRAVITY = 0;               // Gravity
        public double YGRAVITY = 0; //0.15;               // Gravity
        
        private static int FIREWORK_NUM = 50;            // Number of Dot generated each time
        
        private static double X_VELOCITY = 3.5;              // Maximum X Velocity
        private static double Y_VELOCITY = 1.5; //2.5;              // Maximum Y Velocity
        private static int SIZE_MIN = 3;                   // Minimum Size
        private static int SIZE_MAX = 7;                   // Maximum Size

        private List<MagicDot> _fireworks = new List<MagicDot>();

        private DispatcherTimer _timer;                // on enter frame simulator
        private static int FPS = 30;                  // fps of the on enter frame event


        HslColor particleColor = new HslColor();

        public Fireworks()
        {
            InitializeComponent();

//            MouseMove += new MouseEventHandler(Fireworks_MouseMove);
//            MouseEnter += new MouseEventHandler(Fireworks_MouseEnter);
//            MouseLeftButtonDown += new MouseButtonEventHandler(Fireworks_LButtonDown);

            List<Object> array = new List<Object>();
            Object abc = new Object();

            //hslcolor
            particleColor = HslColor.FromColor(Colors.Red);
            particleColor.L *= .75;
        }

        /////////////////////////////////////////////////////        
        // Handlers 
        /////////////////////////////////////////////////////	

        void Fireworks_MouseMove(object sender, MouseEventArgs e)
        {
            //addFirework(e.GetPosition(this).X, e.GetPosition(this).Y);
        }

        void Fireworks_MouseEnter(object sender, MouseEventArgs e)
        {
           // addFirework(e.GetPosition(this).X, e.GetPosition(this).Y);
        }

        void Fireworks_LButtonDown(object sender, MouseButtonEventArgs e)
        {
            addFirework(e.GetPosition(this).X, e.GetPosition(this).Y);
        }

        void _timer_Tick(object sender, EventArgs e)
        {
            moveFirework();

            if (_fireworks.Count == 0)
                Stop();
        }

        /////////////////////////////////////////////////////        
        // Private Methods 
        /////////////////////////////////////////////////////	

        void moveFirework()
        {
            for (int i = _fireworks.Count - 1; i >= 0; i--)
            {
                MagicDot dot = _fireworks[i];
                dot.RunFirework();
                if (dot.Opacity <= 0.1)
                {
                    LayoutRoot.Children.Remove(dot);
                    _fireworks.Remove(dot);
                }
            }
        }

        public void addFirework(double x, double y)
        {
            if (_timer == null)
                Start();

            int seed = (int)DateTime.Now.Ticks;

  
            Color color;// = particleColor.ToColor();

            seed += (int)DateTime.Now.Ticks;
            Random rand = new Random(seed);

            double xVelocity, yVelocity;

            //particleColor.H = rand.Next(255);
            color = Color.FromArgb(255, 217, 251, 255);//particleColor.ToColor();

            for (int i = 0; i < FIREWORK_NUM; i++)
            {
                double size = SIZE_MIN + (SIZE_MAX - SIZE_MIN) * rand.NextDouble();

                byte red = color.R;// (byte)(128 + (128 * r.NextDouble()));
                byte green = color.G;//(byte)(128 + (128 * r.NextDouble()));
                byte blue = color.B;//(byte)(128 + (128 * r.NextDouble()));
                

              /*  double xVelocity = X_VELOCITY - 2 * X_VELOCITY * r.NextDouble();
                double yVelocity = -Y_VELOCITY * r.NextDouble();*/

                xVelocity = Math.Sin((double)i / (double)FIREWORK_NUM * (6.284f)) * X_VELOCITY * (0.8f + (double)(rand.Next() % 40) / 200.0f);
                yVelocity = Math.Cos((double)i / (double)FIREWORK_NUM * (6.284f)) * Y_VELOCITY * (double)(rand.Next() % 50) / 50.0f;// -3;// - 0.5;


                MagicDot dot = new MagicDot(red, green, blue, size);
                dot.X = x;
                dot.Y = y;
                dot.XVelocity = xVelocity;
                dot.YVelocity = yVelocity;
                dot.XGravity = XGRAVITY;
                dot.YGravity = YGRAVITY;
                dot.RunFirework();
                _fireworks.Add(dot);

                LayoutRoot.Children.Add(dot);
            }
        }


        /////////////////////////////////////////////////////        
        // Public Methods 
        /////////////////////////////////////////////////////	

        public void Start()
        {
            Stop();
            _timer = new DispatcherTimer();
            _timer.Interval = new TimeSpan(0, 0, 0, 0, 1000 / FPS);
            _timer.Tick += new EventHandler(_timer_Tick);
            _timer.Start();
        }

        public void Stop()
        {
            if (_timer != null)
            {
                _timer.Stop();
                _timer = null;
            }
        }

        public bool IsComplete()
        {
            if (_fireworks.Count == 0)
                return true;
            return false;

        }
    }
}
