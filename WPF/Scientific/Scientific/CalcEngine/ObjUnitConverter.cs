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


namespace CalcPro
{
    public class TypeUnitConverter
    {
        public string szName { get; set; }
        public string szUnits { get; set; }
    }
    public class TypeSection
    {
        public string szName { get; set; }
        public string szImage { get; set; }
        public byte btFrom = 0;
        public byte btTo = 1;
    }    

    public class ObjUnitConverter: INotifyPropertyChanged
    {
        enum TemperatureType
        {
            TEMP_CELSIUS,
            TEMP_FAHRENHEIT,
            TEMP_KELVIN,
            TEMP_RANKINE,
            TEMP_REAUMUR,
        };

        enum ConvertionSectionType
        {
            CONVERT_ACCELERATION = 0,
            CONVERT_ANGLES,
            CONVERT_AREA,
            CONVERT_COOKING,
            CONVERT_COMPUTER,
            CONVERT_DENSITY,
            CONVERT_ENERGY,
            CONVERT_FORCE,
            CONVERT_FUEL_ECON,
            CONVERT_LENGTH,
            CONVERT_POWER,
            CONVERT_PRESSURE,
            CONVERT_SPEED,
            CONVERT_TEMPERATURE,
            CONVERT_TIME,
            CONVERT_TORQUE,
            CONVERT_VOLUME,
            CONVERT_MASS,//label as weight (mass)
            CONVERT_Count,
        };
        enum UnitDisplayType
        {
            UNIT_DISPLAY_FULL,
            UNIT_DISPLAY_VALUE,
            UNIT_DISPLAY_EXPONENT,
        };

        public event PropertyChangedEventHandler PropertyChanged;
        public string szSection
        {
            get
            {
                return _sSections[_iCurrentSection].szName;
            }
        }

        public string szFromName
        {
            get
            {
                return GetSubSectionName(_sSections[_iCurrentSection].btFrom);
            }
        }

        public string szToName
        {
            get
            {
                return GetSubSectionName(_sSections[_iCurrentSection].btTo);
            }
        }

        public string szFromUnits
        {
            get
            {
                return GetSubSectionUnits(_sSections[_iCurrentSection].btFrom); 
            }
        }

        public string szToUnits
        {
            get
            {
                return GetSubSectionUnits(_sSections[_iCurrentSection].btTo);
            }
        }
                 
        public string szFrom
        {
            get
            {
                return Globals.Display.szResult; 
            }
        }
        public string szTo
        {
            get
            {
                if (Globals.Display.szResult.Length == 0)
                    return "0.";
                return GetResult(Globals.Display.szResult); 
            }
        }

        public int iCurrentSection
        {
            get { return _iCurrentSection; }
        }
        public int iCurrentFrom
        {
            get {return (int)_sSections[_iCurrentSection].btFrom;}
        }
        public int iCurrentTo
        {
            get {return (int)_sSections[_iCurrentSection].btTo;}
        }

        private const int NUMBER_OF_LENGTH = 14;
        private const int NUMBER_OF_TEMP = 5;
        private const int NUMBER_OF_SPEED = 12;
        private const int NUMBER_OF_VOLUME = 21;
        private const int NUMBER_OF_COOKING = 15;
        private const int NUMBER_OF_COMP = 17;
        private const int NUMBER_OF_AREA = 13;
        private const int NUMBER_OF_ENERGY = 13;
        private const int NUMBER_OF_POWER = 18;
        private const int NUMBER_OF_MASS = 12;
        private const int NUMBER_OF_FORCE = 12;
        private const int NUMBER_OF_PRESSURE = 21;
        private const int NUMBER_OF_TIME = 13;
        private const int NUMBER_OF_FUELECON = 12;
        private const int NUMBER_OF_ANGLES = 9;
        private const int NUMBER_OF_TORQUE = 8;
        private const int NUMBER_OF_DENSITY = 9;
        private const int NUMBER_OF_ACCELERATION = 6;
        private int _iCurrentSection;
        //private bool _bSwitchFromTo;
        //private string szResult;
        private double[] _dbLength;
    	private double[] _dbSpeed;
    	private double[] _dbVolume;
    	private double[] _dbCooking;
    	private double[] _dbComputers;
    	private double[] _dbArea;
    	private double[] _dbEnergy;
    	private double[] _dbPower;
    	private double[] _dbMass;
    	private double[] _dbForce;
    	private double[] _dbPressure;
    	private double[] _dbFuelEcon;
    	private double[] _dbTime;
        private double[] _dbAngles;
        private double[] _dbTorque;
        private double[] _dbDensity;
        private double[] _dbAcceleration;
        //private UnitDisplayType _eUnitDisplay;
        private TypeSection[] _sSections;
        private TypeUnitConverter[] _sAcceleration;
        private TypeUnitConverter[] _sDensity;
        private TypeUnitConverter[] _sAngles;
        private TypeUnitConverter[] _sTorque;
        private TypeUnitConverter[] _sLength;
        private TypeUnitConverter[] _sTemp;
        private TypeUnitConverter[] _sSpeed;
        private TypeUnitConverter[] _sVolume;
        private TypeUnitConverter[] _sCooking;
        private TypeUnitConverter[] _sComputers;
        private TypeUnitConverter[] _sArea;
        private TypeUnitConverter[] _sEnergy;
        private TypeUnitConverter[] _sPower;
        private TypeUnitConverter[] _sMass;
        private TypeUnitConverter[] _sForce;
        private TypeUnitConverter[] _sPressure;
        private TypeUnitConverter[] _sFuelEcon;
        private TypeUnitConverter[] _sTime;

        private double dbOut = 0.0;//so we can switch ... used in GetResult

        public ObjUnitConverter()
        {
            _dbAcceleration = new double[NUMBER_OF_ACCELERATION]
            {
                1.0, //meters per second squared
                0.1019716213,//gravity
                3.280839895,//feet per second squared
                39.37007874,//inches per s squared
                100.0,//gals
                0.1,//leos
            };

            _dbDensity = new double [NUMBER_OF_DENSITY]
            {
                0.001,//gram cm^3
                1000,//gram m^3
                1.0,//gram/l
                1.0,//kg m^3
                0.000578036672,//ounce inch
                0.99884736919,//ounce ft
                0.000036127292,//pound /inch
                0.062427960576,//pound /ft^3
                0.0010018032458,//water (20)
            };

            _dbAngles = new double [NUMBER_OF_ANGLES]
            {
                360.0,//degrees
                6.2831853072, //radians
                400.0,//gradians
                21600.0,//minutes
                1296000.0,//seconds
                1.0,//circles
                4.0,//quadrants
                32.0,//points
                6400.0,//mil
            };

            _dbTorque = new double[NUMBER_OF_TORQUE]
            {
                1.355817952,//newton-meter
                1.0,//foot pound
                0.13825495475,//kilogram-meter
                13.825495475,//kg-cm
                13825.495475,//g-cm
                12.0,//inch pound
                192.0,//inch oz     
                13558179.52,//dyne centimeter
            };

            _dbLength = new double [NUMBER_OF_LENGTH]
            {   
                0.001,//km
		        1,//meter
                100,//cm
		        10,//dm
		        1000,//millimeter
                1000000000,//nm
                0.00017998560115,//league (nautical)
		        0.00062137119224,//mile
		        0.000539956803,//nautical miles
		        1.0936132983,//yard
                3.280839895,//ft
                39.37007874,//inch
                39370078.74,//microinch
                6.6845871227*Math.Pow(10.0,-12.0),//astronomical units
	        };

            _dbSpeed = new double [NUMBER_OF_SPEED]
            {
		        196850.3937,//		ft/min		
		        3280.839895,//		ft/s		
		        39370.07874, //		inch/s
		        3600.0,//		km/h
                60.0,//km/min
		        1.0,//		km/s
		        1943.8444925,//		knots			
		        60000.0,//		m/min
		        1000.0,//		m/s			
		        2236.9362921,//		mph				
		        0.000003335640952,//		c	
		        2.938669958//		mach		
	        };

            _dbVolume = new double[NUMBER_OF_VOLUME]
            {
                1.0,		    //gal US
	            0.85936700169,	//Gal dry US
	            0.83267418463,	//gallons UK
	            4.0,		    //quarts US
	            3.4374680068,	//quarts dry
	            3.3306967385,	//quarts UK
	            8.0,		    //
	            6.8749360135,	
	            6.661393477,	//pints
	            16.0,	
	            15.141647136,	//cups
	            128.0,
	            133.2278701,    //ounces
	            0.13368055556 ,	//cubic feet
	            231.0,		    //cubic inches
	            3.785411784,	//liters
	            378.5411784,	//cl
	            3785.411784,	//ml
	            0.003785411784, //m3
	            3785.411784,	//cm3
	            3785411.784     //mm3
            };	

            _dbCooking = new double [NUMBER_OF_COOKING]
            {
                1.0,	
	            0.83267418463,
	            4.0,
	            3.3306967385,
	            16.0,
	            15.141647136,
	            128.0,
	            133.22786954,
	            256.0,
	            768.0,
	            3.785411784,//liters
	            378.5411784,//centimeters
	            3785.411784,//millimeters
	            8.0,//pints
	            6.661393477//pint UK
	        };

            _dbComputers = new double [NUMBER_OF_COMP]
            {
                1.0/8.0,	//bit
	            1,	        //byte
	            Math.Pow(2.0,10.0),	//kilobyte
	            Math.Pow(2.0,20.0),	//megabyte
	            Math.Pow(2.0,30.0),
	            Math.Pow(2.0,40.0), 
	            Math.Pow(2.0,50.0), 
	            Math.Pow(2.0,60.0),
	            8.0,
	            16.0,	
	            32.0,	
	            1457664.0,//floppy 3.5
	            681058304.0,	//72cd
	            736279247.0,	//80cd
	            4700000000.0,//dvd-r
                8500000000.0,//dvd-r DL
                4700372992.0//dvd+r
	        };

            _dbArea = new double [NUMBER_OF_AREA]
            {
		        43560.0, //acres
		        107639.10417,	//hectares
		        10763910.417,	//km^2
                10.763910417,	//m^2
                0.0010763910417,//cm^2
		        0.000010763910417,  //mm^2
                27878400.0, //miles^2
                9.0,	//yards^2
                1.0, //feet^2
		        1.0/144.0,	//inches^2
                27878511.833,//mile survey
                1.0000040042,//feet survey
                0.0069444722512//inch survey
	        };

            _dbEnergy = new double [NUMBER_OF_ENERGY]
            {
		        0.00094845138281,	//BTU
		        0.23900573614,	//calories (thermo)
		        0.73756217557, //foot pounds
		        3.7250614123*Math.Pow(10.0,-7.0), //hp/h
		        1.0,//joules
		        0.00023900573614,	//kilocals
		        0.1019716213,//kg/m
		        0.001,//kilojoules
		        0.000000277777777778,	//kilwatt hours
		        9.4804342797*Math.Pow(10.0,-9.0), //therms
		        0.000277777777778,	//watt hours,
		        1.0,	//watt seconds
                10000000.0,//ergs
	        };

            _dbPower = new double [NUMBER_OF_POWER]
            {
                0.000000000001,//terrawatts
                0.000000001,	//gigawatts
                0.000001,	//mwatts
		        0.001, //kwatts
		        1.0,	//watts
		        1000.0, //milliwatt

                0.0013596216173,	//HP metric
                0.0013410220924,	//HP IT
        
                0.23884589663,	//cals/s
                14.330753798,	//cals/min
                859.84522786,	//cals/hour
		        0.00094781712087,	//BTU/sec
                0.056869027252,	//BTU/min
		        3.4121416351,	//BTU/hour
		
		        0.73756217557,//ft-pounds/sec
                44.253730534,			//ft-pound/min
		        2655.2238321,//ft-pounds/hour
                0.00028434513609399,//kW to Tons of refrigeration.
	        };

            _dbMass = new double [NUMBER_OF_MASS]
            {
		        1000.0,//grams
		        1.0,	//kg
		        35.273961949580412915675808215204,//ounces
                32.150746568627980522100346029483, //ounces Troy
		        2.2046226218487758072297380134503,	//pounds
                2.0094216605392487826312716268425,//pound Troy
		        0.001,	//tonnes metric
		        0.0011023113109243879036148690067251,	//Tons US Short ... 5000 pounds
		        0.00098420652761106062822756161314744,//Tons UK Long
		        0.15747304441776970051640985810359,	//Stones
		        0.022046226218487758072297380134503,	//Hun We US short
		        0.019684130552    //Hun We UK long
	        };

            _dbForce = new double [NUMBER_OF_FORCE]
            {
		        100000000000,//dynes
		        101971621.2978, //g
		        101971.6212978,//Kg
		        1000.0,	//knewts
		        1.0,	//megnewts
		        1000000.0,	//newtons
		        3596943.1019,//ounce
		        224808.943871,	//pounds
		        101.9716212978,	//tonnes
		        112.4044719,	//tonnes US
		        100.3611357,	//tonnes UK
                224.80894387//kips
	        };

            _dbPressure = new double [NUMBER_OF_PRESSURE]
            {
		        10000000.0,	//kg  m
                1000.0,		//kg cm
                10.0,		//kg mm
                98066500.0,	//N m
		        9806.65,	//N cm	
                98.0665,	//N mm
                980.665,//bars	
                980665,		//minibars	
                98066500.0,	 //pasc
                980665, //hectopasc
                967.84100984,	//atm
                735592.3136,//torr	
                28959.020848, //inches merc
                2048161.4401, //pounds ft
                14223.343334,	//pounds inch
                1024.0807201 , //tons foot US
                7.1116716671,	//tons inch 
                914.35778578 ,	//tons foot uk	
                6.3497068457 ,	//tonnes inch uk
                10000.0,	//tonnes meter
                1.0,		//tonnes ce=m	
	        };

            _dbFuelEcon = new double [NUMBER_OF_FUELECON]
            {   //so the positive numbers are relevant to each other as are the negatives ... but pos and neg have no correlation
		        -1.0,//l/100km
                235.2145833,//miles/galon US
		        282.4809363,//miles/gallon UK
                378.5411784,//km/gallon
                454.609,//km/gallon UK		
		        62.1371192,//miles/liter
		        100.0,//km/liter
		        -0.4251437,//gallons US/100 miles
		        -0.2641721,//gallons US/100 km
		        -0.3540062,//gallons UK/100 miles
		        -0.2199692,//gallons UK 100 km
		        -1.609344,//liters / 100 miles
	        };

            _dbTime = new double [NUMBER_OF_TIME]
            {
		        86400000000000.0,//nanosecond
		        86400000.0,//millisecond
		        86400.0,//sec
		        1440.0,//min
		        24.0,//hour
		        1.0,//day
		        3.0/5.0,//work week
                1.0/7.0,//week
                1.0/30.4368499,//month (assume 30 days)
		        1.0/365.24219878,//year
		        1.0/3652.4219878,//decade
		        1.0/36524.219878,//century
		        1.0/365242.19878 ,//millennium
	        };

            ReloadText();
            
            _iCurrentSection = 0;





        }
        public bool ReloadText()
        {
            ResourceManager rm = new ResourceManager("CalcPro.Strings.UnitConverterRes", Assembly.GetExecutingAssembly());

            _sSections = new TypeSection[(int)ConvertionSectionType.CONVERT_Count];
            for(int i=0; i<(int)ConvertionSectionType.CONVERT_Count; i++) _sSections[i] = new TypeSection();
            _sSections[(int)ConvertionSectionType.CONVERT_LENGTH].szName = rm.GetString("IDS_UNIT_Distance");
            _sSections[(int)ConvertionSectionType.CONVERT_TEMPERATURE].szName = rm.GetString("IDS_UNIT_Temperature");
            _sSections[(int)ConvertionSectionType.CONVERT_SPEED].szName = rm.GetString("IDS_UNIT_Speed");
            _sSections[(int)ConvertionSectionType.CONVERT_VOLUME].szName = rm.GetString("IDS_UNIT_Volume");
            _sSections[(int)ConvertionSectionType.CONVERT_MASS].szName = rm.GetString("IDS_UNIT_Mass");
            _sSections[(int)ConvertionSectionType.CONVERT_COOKING].szName = rm.GetString("IDS_UNIT_Cooking");
            _sSections[(int)ConvertionSectionType.CONVERT_COMPUTER].szName = rm.GetString("IDS_UNIT_Computers");
            _sSections[(int)ConvertionSectionType.CONVERT_AREA].szName = rm.GetString("IDS_UNIT_Area");
            _sSections[(int)ConvertionSectionType.CONVERT_ENERGY].szName = rm.GetString("IDS_UNIT_Energy");
            _sSections[(int)ConvertionSectionType.CONVERT_POWER].szName = rm.GetString("IDS_UNIT_Power");
            _sSections[(int)ConvertionSectionType.CONVERT_FORCE].szName = rm.GetString("IDS_UNIT_Force");
            _sSections[(int)ConvertionSectionType.CONVERT_PRESSURE].szName = rm.GetString("IDS_UNIT_Pressure");
            _sSections[(int)ConvertionSectionType.CONVERT_FUEL_ECON].szName = rm.GetString("IDS_UNIT_FuelEcon");
            _sSections[(int)ConvertionSectionType.CONVERT_TIME].szName = rm.GetString("IDS_UNIT_Time");
            _sSections[(int)ConvertionSectionType.CONVERT_ANGLES].szName = rm.GetString("IDS_UNIT_Angles");
            _sSections[(int)ConvertionSectionType.CONVERT_TORQUE].szName = rm.GetString("IDS_UNIT_Torque");
            _sSections[(int)ConvertionSectionType.CONVERT_DENSITY].szName = rm.GetString("IDS_UNIT_Density");
            _sSections[(int)ConvertionSectionType.CONVERT_ACCELERATION].szName = rm.GetString("IDS_UNIT_Acceleration");

            _sSections[(int)ConvertionSectionType.CONVERT_LENGTH].szImage = "/Assets/Unit/Icon_length.png";
            _sSections[(int)ConvertionSectionType.CONVERT_TEMPERATURE].szImage = "/Assets/Unit/Icon_temp.png";
            _sSections[(int)ConvertionSectionType.CONVERT_SPEED].szImage = "/Assets/Unit/Icon_speed.png";
            _sSections[(int)ConvertionSectionType.CONVERT_VOLUME].szImage = "/Assets/Unit/Icon_volume.png";
            _sSections[(int)ConvertionSectionType.CONVERT_MASS].szImage = "/Assets/Unit/Icon_weight.png";
            _sSections[(int)ConvertionSectionType.CONVERT_COOKING].szImage = "/Assets/Unit/Icon_cooking.png";
            _sSections[(int)ConvertionSectionType.CONVERT_COMPUTER].szImage = "/Assets/Unit/Icon_data.png";
            _sSections[(int)ConvertionSectionType.CONVERT_AREA].szImage = "/Assets/Unit/Icon_area.png";
            _sSections[(int)ConvertionSectionType.CONVERT_ENERGY].szImage = "/Assets/Unit/Icon_energy.png";
            _sSections[(int)ConvertionSectionType.CONVERT_POWER].szImage = "/Assets/Unit/Icon_power.png";
            _sSections[(int)ConvertionSectionType.CONVERT_FORCE].szImage = "/Assets/Unit/Icon_force.png";
            _sSections[(int)ConvertionSectionType.CONVERT_PRESSURE].szImage = "/Assets/Unit/Icon_pressure.png";
            _sSections[(int)ConvertionSectionType.CONVERT_FUEL_ECON].szImage = "/Assets/Unit/Icon_fuel.png";
            _sSections[(int)ConvertionSectionType.CONVERT_TIME].szImage = "/Assets/Unit/Icon_time.png";
            _sSections[(int)ConvertionSectionType.CONVERT_ANGLES].szImage = "/Assets/Unit/Icon_angle.png";
            _sSections[(int)ConvertionSectionType.CONVERT_TORQUE].szImage = "/Assets/Unit/Icon_torque.png";
            _sSections[(int)ConvertionSectionType.CONVERT_DENSITY].szImage = "/Assets/Unit/Icon_density.png";
            _sSections[(int)ConvertionSectionType.CONVERT_ACCELERATION].szImage = "/Assets/Unit/Icon_acel.png";

            _sAcceleration = new TypeUnitConverter[NUMBER_OF_ACCELERATION];
            for (int i = 0; i < NUMBER_OF_ACCELERATION; i++) _sAcceleration[i] = new TypeUnitConverter();
            _sAcceleration[0].szName = rm.GetString("IDS_UACC_MetersPerSecondSq");
            _sAcceleration[0].szUnits = rm.GetString("IDS_UACC_MPS2");
            _sAcceleration[1].szName = rm.GetString("IDS_UACC_Gravity");
            _sAcceleration[1].szUnits = rm.GetString("IDS_UACC_g");
            _sAcceleration[2].szName = rm.GetString("IDS_UACC_FeetPerSecondSq");
            _sAcceleration[2].szUnits = rm.GetString("IDS_UACC_FPS2");
            _sAcceleration[3].szName = rm.GetString("IDS_UACC_InPerSecSq");
            _sAcceleration[3].szUnits = rm.GetString("IDS_UACC_IPS2");
            _sAcceleration[4].szName = rm.GetString("IDS_UACC_Galileos");
            _sAcceleration[4].szUnits = rm.GetString("IDS_UACC_gal");
            _sAcceleration[5].szName = rm.GetString("IDS_UACC_Leos");
            _sAcceleration[5].szUnits = rm.GetString("IDS_UACC_leo");

            _sDensity = new TypeUnitConverter[NUMBER_OF_DENSITY];
            for (int i = 0; i < NUMBER_OF_DENSITY; i++) _sDensity[i] = new TypeUnitConverter();
            _sDensity[0].szName = rm.GetString("IDS_UD_GramsPerCm3");
            _sDensity[0].szUnits = rm.GetString("IDS_UD_GPC3");
            _sDensity[1].szName = rm.GetString("IDS_UD_GramsPerM3");
            _sDensity[1].szUnits = rm.GetString("IDS_UD_GPM3");
            _sDensity[2].szName = rm.GetString("IDS_UD_GramsPerLiter");
            _sDensity[2].szUnits = rm.GetString("IDS_UD_GPL");
            _sDensity[3].szName = rm.GetString("IDS_UD_KgPerM3");
            _sDensity[3].szUnits = rm.GetString("IDS_UD_KPM3");
            _sDensity[4].szName = rm.GetString("IDS_UD_OuncesPerInch3");
            _sDensity[4].szUnits = rm.GetString("IDS_UD_OPI3");
            _sDensity[5].szName = rm.GetString("IDS_UD_OuncesPerFoot3");
            _sDensity[5].szUnits = rm.GetString("IDS_UD_OPF3");
            _sDensity[6].szName = rm.GetString("IDS_UD_PoundsPerInch3");
            _sDensity[6].szUnits = rm.GetString("IDS_UD_PPI3");
            _sDensity[7].szName = rm.GetString("IDS_UD_PoundsPerFoot3");
            _sDensity[7].szUnits = rm.GetString("IDS_UD_PPF3");
            _sDensity[8].szName = rm.GetString("IDS_UD_WaterAt20C");
            _sDensity[8].szUnits = rm.GetString("IDS_UD_H2OAt20C");

            _sAngles = new TypeUnitConverter[NUMBER_OF_ANGLES];
            for (int i = 0; i < NUMBER_OF_ANGLES; i++) _sAngles[i] = new TypeUnitConverter();
            _sAngles[0].szName = rm.GetString("IDS_UAN_Degrees");
            _sAngles[0].szUnits = rm.GetString("IDS_UAN_deg");
            _sAngles[1].szName = rm.GetString("IDS_UAN_Radians");
            _sAngles[1].szUnits = rm.GetString("IDS_UAN_rad");
            _sAngles[2].szName = rm.GetString("IDS_UAN_Gradians");
            _sAngles[2].szUnits = rm.GetString("IDS_UAN_grad");
            _sAngles[3].szName = rm.GetString("IDS_UT_Minute");
            _sAngles[3].szUnits = rm.GetString("IDS_UT_min");
            _sAngles[4].szName = rm.GetString("IDS_UT_Sec");
            _sAngles[4].szUnits = rm.GetString("IDS_UT_s");
            _sAngles[5].szName = rm.GetString("IDS_UAN_Circle");
            _sAngles[5].szUnits = rm.GetString("IDS_UAN_cir");
            _sAngles[6].szName = rm.GetString("IDS_UAN_Quadrants");
            _sAngles[6].szUnits = rm.GetString("IDS_UAN_quad");
            _sAngles[7].szName = rm.GetString("IDS_UAN_Point");
            _sAngles[7].szUnits = rm.GetString("IDS_UAN_pts");
            _sAngles[8].szName = rm.GetString("IDS_UAN_Mil");
            _sAngles[8].szUnits = rm.GetString("IDS_UAN_mill");

            _sTorque = new TypeUnitConverter[NUMBER_OF_TORQUE];
            for (int i = 0; i < NUMBER_OF_TORQUE; i++) _sTorque[i] = new TypeUnitConverter();
            _sTorque[0].szName = "Newton·Meter";
            _sTorque[0].szUnits ="N·m";
            _sTorque[1].szName = "Foot·Pound";
            _sTorque[1].szUnits ="ft·lbs";
            _sTorque[2].szName = "Kilogram·Meter";
            _sTorque[2].szUnits ="kg·m";
            _sTorque[3].szName = "Kilogram·Centimeter";
            _sTorque[3].szUnits = "kg·cm";
            _sTorque[4].szName = "Gram·Centimeter";
            _sTorque[4].szUnits ="g·cm";
            _sTorque[5].szName = "Inch·Pound";
            _sTorque[5].szUnits ="in·lbs";
            _sTorque[6].szName = "Inch·Ounce";
            _sTorque[6].szUnits ="in·oz";
            _sTorque[7].szName = "Dyne·Centimeter";
            _sTorque[7].szUnits ="dyne·cm";

            _sLength = new TypeUnitConverter[NUMBER_OF_LENGTH];
            for (int i = 0; i < NUMBER_OF_LENGTH; i++) _sLength[i] = new TypeUnitConverter();
            _sLength[0].szName = rm.GetString("IDS_UL_kilometers");
            _sLength[0].szUnits = rm.GetString("IDS_UL_km");
            _sLength[1].szName = rm.GetString("IDS_UL_meters");
            _sLength[1].szUnits = rm.GetString("IDS_UL_m");
            _sLength[2].szName = rm.GetString("IDS_UL_centimeters");
            _sLength[2].szUnits = rm.GetString("IDS_UL_cm");
            _sLength[3].szName = rm.GetString("IDS_UL_decimeters");
            _sLength[3].szUnits = rm.GetString("IDS_UL_dm");
            _sLength[4].szName = rm.GetString("IDS_UL_millimeters");
            _sLength[4].szUnits = rm.GetString("IDS_UL_mm");
            _sLength[5].szName = rm.GetString("IDS_UL_nanometers");
            _sLength[5].szUnits = rm.GetString("IDS_UL_nanm");
            _sLength[6].szName = rm.GetString("IDS_UL_leagues");
            _sLength[6].szUnits = rm.GetString("IDS_UL_leg");
            _sLength[7].szName = rm.GetString("IDS_UL_miles");
            _sLength[7].szUnits = rm.GetString("IDS_UL_mi");
            _sLength[8].szName = rm.GetString("IDS_UL_nautical_miles");
            _sLength[8].szUnits = rm.GetString("IDS_UL_nm");
            _sLength[9].szName = rm.GetString("IDS_UL_yards");
            _sLength[9].szUnits = rm.GetString("IDS_UL_yd");
            _sLength[10].szName = rm.GetString("IDS_UL_feet");
            _sLength[10].szUnits = rm.GetString("IDS_UL_ft");
            _sLength[11].szName = rm.GetString("IDS_UL_inches");
            _sLength[11].szUnits = rm.GetString("IDS_UL_in");
            _sLength[12].szName = rm.GetString("IDS_UL_Microinches");
            _sLength[12].szUnits = rm.GetString("IDS_UL_min");
            _sLength[13].szName = rm.GetString("IDS_UL_AstronomicalUnits");
            _sLength[13].szUnits = rm.GetString("IDS_UL_AU");

            //Temperature Initialize/////////////////////////////////////////////////////////////////////////////////////
            _sTemp = new TypeUnitConverter[NUMBER_OF_TEMP];
            for (int i = 0; i < NUMBER_OF_TEMP; i++) _sTemp[i] = new TypeUnitConverter();
            _sTemp[0].szName = rm.GetString("IDS_UT_Celcius");
            _sTemp[0].szUnits = rm.GetString("IDS_UT_C");
            _sTemp[1].szName = rm.GetString("IDS_UT_Fahrenheit");
            _sTemp[1].szUnits = rm.GetString("IDS_UT_F");
            _sTemp[2].szName = rm.GetString("IDS_UT_Kelvin");
            _sTemp[2].szUnits = rm.GetString("IDS_UT_K");
            _sTemp[3].szName = rm.GetString("IDS_UT_Rankine");
            _sTemp[3].szUnits = rm.GetString("IDS_UT_R");
            _sTemp[4].szName = "Réaumur";
            _sTemp[4].szUnits = "°Ré";
            //end Temperature/////////////////////////////////////////////////////////////////////////////////////////////


            //Speed Initialize////////////////////////////////////////////////////////////////////////////////////////////
            _sSpeed = new TypeUnitConverter[NUMBER_OF_SPEED];
            for (int i = 0; i < NUMBER_OF_SPEED; i++) _sSpeed[i] = new TypeUnitConverter();
            _sSpeed[0].szName = rm.GetString("IDS_US_Feet_per_Minute");
            _sSpeed[0].szUnits = rm.GetString("IDS_US_ft_min");
            _sSpeed[1].szName = rm.GetString("IDS_US_Feet_per_Second");
            _sSpeed[1].szUnits = rm.GetString("IDS_US_f_s");
            _sSpeed[2].szName = rm.GetString("IDS_US_Inch_per_Second");
            _sSpeed[2].szUnits = rm.GetString("IDS_US_in_s");
            _sSpeed[3].szName = rm.GetString("IDS_US_Kilometers_per_Hour");
            _sSpeed[3].szUnits = rm.GetString("IDS_US_km_h");
            _sSpeed[4].szName = rm.GetString("IDS_US_Kilometers_per_Minute");
            _sSpeed[4].szUnits = rm.GetString("IDS_US_km_min");
            _sSpeed[5].szName = rm.GetString("IDS_US_Kilometers_per_Second");
            _sSpeed[5].szUnits = rm.GetString("IDS_US_km_s");
            _sSpeed[6].szName = rm.GetString("IDS_US_Knots");
            _sSpeed[6].szUnits = rm.GetString("IDS_US_nmph");
            _sSpeed[7].szName = rm.GetString("IDS_US_Meters_per_Minute");
            _sSpeed[7].szUnits = rm.GetString("IDS_US_m_min");
            _sSpeed[8].szName = rm.GetString("IDS_US_Meters_per_Second");
            _sSpeed[8].szUnits = rm.GetString("IDS_US_m_s");
            _sSpeed[9].szName = rm.GetString("IDS_US_Miles_per_Hour");
            _sSpeed[9].szUnits = rm.GetString("IDS_US_mph");
            _sSpeed[10].szName = rm.GetString("IDS_US_Speed_of_Light");
            _sSpeed[10].szUnits = rm.GetString("IDS_US_c");
            _sSpeed[11].szName = rm.GetString("IDS_US_Speed_of_Sound");
            _sSpeed[11].szUnits = rm.GetString("IDS_US_Mach");
            //end speed//////////////////////////////////////////////////////////////////////////////////////////////////

            //Volume Conversion Initialize//////////////////////////////////////////////////////////////////////////////
            _sVolume = new TypeUnitConverter[NUMBER_OF_VOLUME];
            for (int i = 0; i < NUMBER_OF_VOLUME; i++) _sVolume[i] = new TypeUnitConverter();
            _sVolume[0].szName = rm.GetString("IDS_UV_Gallons_liquid_US");
            _sVolume[0].szUnits = rm.GetString("IDS_UV_gal_US");
            _sVolume[1].szName = rm.GetString("IDS_UV_Gallons_dry_US");
            _sVolume[1].szUnits = rm.GetString("IDS_UV_gal_US_dry");
            _sVolume[2].szName = rm.GetString("IDS_UV_Gallons_UK");
            _sVolume[2].szUnits = rm.GetString("IDS_UV_gal_UK");
            _sVolume[3].szName = rm.GetString("IDS_UV_Quarts_liquid_US");
            _sVolume[3].szUnits = rm.GetString("IDS_UV_qt_US");
            _sVolume[4].szName = rm.GetString("IDS_UV_Quarts_dry_US");
            _sVolume[4].szUnits = rm.GetString("IDS_UV_qt_US_dry");
            _sVolume[5].szName = rm.GetString("IDS_UV_Quarts_UK");
            _sVolume[5].szUnits = rm.GetString("IDS_UV_qt_UK");
            _sVolume[6].szName = rm.GetString("IDS_UV_Pints_liquid_US");
            _sVolume[6].szUnits = rm.GetString("IDS_UV_pt_US");
            _sVolume[7].szName = rm.GetString("IDS_UV_Pints_dry_US");
            _sVolume[7].szUnits = rm.GetString("IDS_UV_pt_US_dry");
            _sVolume[8].szName = rm.GetString("IDS_UV_Pints_UK");
            _sVolume[8].szUnits = rm.GetString("IDS_UV_pt_UK");
            _sVolume[9].szName = rm.GetString("IDS_UV_Cups_US");
            _sVolume[9].szUnits = rm.GetString("IDS_UV_c_US");
            _sVolume[10].szName = rm.GetString("IDS_UV_Cups_metric");
            _sVolume[10].szUnits = rm.GetString("IDS_UV_c");
            _sVolume[11].szName = rm.GetString("IDS_UV_Fluid_Ounces_US");
            _sVolume[11].szUnits = rm.GetString("IDS_UV_fl_oz_US");
            _sVolume[12].szName = rm.GetString("IDS_UV_Fluid_Ounces_UK");
            _sVolume[12].szUnits = rm.GetString("IDS_UV_fl_oz_UK");
            _sVolume[13].szName = rm.GetString("IDS_UV_Cubic_feet");
            _sVolume[13].szUnits = rm.GetString("IDS_UV_ft");
            _sVolume[14].szName = rm.GetString("IDS_UV_Cubic_Inches");
            _sVolume[14].szUnits = rm.GetString("IDS_UV_in");
            _sVolume[15].szName = rm.GetString("IDS_UV_Liters");
            _sVolume[15].szUnits = rm.GetString("IDS_UV_l");
            _sVolume[16].szName = rm.GetString("IDS_UV_Centiliters");
            _sVolume[16].szUnits = rm.GetString("IDS_UV_cl");
            _sVolume[17].szName = rm.GetString("IDS_UV_milliliters");
            _sVolume[17].szUnits = rm.GetString("IDS_UV_ml");
            _sVolume[18].szName = rm.GetString("IDS_UV_Cubic_meters");
            _sVolume[18].szUnits = rm.GetString("IDS_UV_m");
            _sVolume[19].szName = rm.GetString("IDS_UV_Cubic_centimeters");
            _sVolume[19].szUnits = rm.GetString("IDS_UV_cm");
            _sVolume[20].szName = rm.GetString("IDS_UV_Cubic_millimeters");
            _sVolume[20].szUnits = rm.GetString("IDS_UV_mm");
            //end Volume//////////////////////////////////////////////////////////////////////////////////////

            //Cooking Conversion Intialize//////////////////////////////////////////////////////////////////////////////
            _sCooking = new TypeUnitConverter[NUMBER_OF_COOKING];
            for (int i = 0; i < NUMBER_OF_COOKING; i++) _sCooking[i] = new TypeUnitConverter();
            _sCooking[0].szName = rm.GetString("IDS_UC_Gallons_liquid_US");
            _sCooking[0].szUnits = rm.GetString("IDS_UC_gal_US");
            _sCooking[1].szName = rm.GetString("IDS_UC_Gallons_UK");
            _sCooking[1].szUnits = rm.GetString("IDS_UC_gal_UK");
            _sCooking[2].szName = rm.GetString("IDS_UC_Quarts_liquid_US");
            _sCooking[2].szUnits = rm.GetString("IDS_UC_qt_US");
            _sCooking[3].szName = rm.GetString("IDS_UC_Quarts_UK");
            _sCooking[3].szUnits = rm.GetString("IDS_UC_qt_UK");
            _sCooking[4].szName = rm.GetString("IDS_UC_Cups_US");
            _sCooking[4].szUnits = rm.GetString("IDS_UC_c_US");
            _sCooking[5].szName = rm.GetString("IDS_UC_Cups_metric");
            _sCooking[5].szUnits = rm.GetString("IDS_UC_c");
            _sCooking[6].szName = rm.GetString("IDS_UC_Fluid_Ounces_US");
            _sCooking[6].szUnits = rm.GetString("IDS_UC_fl_oz_US");
            _sCooking[7].szName = rm.GetString("IDS_UC_Fluid_Ounces_UK");
            _sCooking[7].szUnits = rm.GetString("IDS_UC_fl_oz_UK");
            _sCooking[8].szName = rm.GetString("IDS_UC_Tablespoon");
            _sCooking[8].szUnits = rm.GetString("IDS_UC_tbsp");
            _sCooking[9].szName = rm.GetString("IDS_UC_Teaspoon");
            _sCooking[9].szUnits = rm.GetString("IDS_UC_tsp");
            _sCooking[10].szName = rm.GetString("IDS_UC_Liters");
            _sCooking[10].szUnits = rm.GetString("IDS_UC_l");
            _sCooking[11].szName = rm.GetString("IDS_UV_Centiliters");
            _sCooking[11].szUnits = rm.GetString("IDS_UV_cl");
            _sCooking[12].szName = rm.GetString("IDS_UC_milliliters");
            _sCooking[12].szUnits = rm.GetString("IDS_UC_ml");
            _sCooking[13].szName = rm.GetString("IDS_UC_Pints_US");
            _sCooking[13].szUnits = rm.GetString("IDS_UC_p_US");
            _sCooking[14].szName = rm.GetString("IDS_UC_Pints_UK");
            _sCooking[14].szUnits = rm.GetString("IDS_UC_p_UK");
            //end Cooking//////////////////////////////////////////////////////////////////////////////////////




            //Computer Conversion Intialize//////////////////////////////////////////////////////////////////////////////
            _sComputers = new TypeUnitConverter[NUMBER_OF_COMP];
            for (int i = 0; i < NUMBER_OF_COMP; i++) _sComputers[i] = new TypeUnitConverter();
            _sComputers[0].szName = rm.GetString("IDS_UC_bit");
            _sComputers[0].szUnits = rm.GetString("IDS_UC_bit");
            _sComputers[1].szName = rm.GetString("IDS_UC_byte");
            _sComputers[1].szUnits = rm.GetString("IDS_UC_byte");
            _sComputers[2].szName = rm.GetString("IDS_UC_kilobyte");
            _sComputers[2].szUnits = rm.GetString("IDS_UC_Kb");
            _sComputers[3].szName = rm.GetString("IDS_UC_megabyte");
            _sComputers[3].szUnits = rm.GetString("IDS_UC_MB");
            _sComputers[4].szName = rm.GetString("IDS_UC_gigabyte");
            _sComputers[4].szUnits = rm.GetString("IDS_UC_GB");
            _sComputers[5].szName = rm.GetString("IDS_UC_terabyte");
            _sComputers[5].szUnits = rm.GetString("IDS_UC_TB");
            _sComputers[6].szName = rm.GetString("IDS_UC_petabyte");
            _sComputers[6].szUnits = rm.GetString("IDS_UC_PB");
            _sComputers[7].szName = rm.GetString("IDS_UC_exabyte");
            _sComputers[7].szUnits = rm.GetString("IDS_UC_EB");
            _sComputers[8].szName = rm.GetString("IDS_UC_Word");
            _sComputers[8].szUnits = rm.GetString("IDS_UC_Word");
            _sComputers[9].szName = rm.GetString("IDS_UC_DWord");
            _sComputers[9].szUnits = rm.GetString("IDS_UC_DWord");
            _sComputers[10].szName = rm.GetString("IDS_UC_QWord");
            _sComputers[10].szUnits = rm.GetString("IDS_UC_QWord");
            _sComputers[11].szName = rm.GetString("IDS_UC_35Floppy");
            _sComputers[11].szUnits = rm.GetString("IDS_UC_disc");
            _sComputers[12].szName = rm.GetString("IDS_UC_CD74");
            _sComputers[12].szUnits = rm.GetString("IDS_UC_CD74_2");
            _sComputers[13].szName = rm.GetString("IDS_UC_CD80");
            _sComputers[13].szUnits = rm.GetString("IDS_UC_CD80_2");
            _sComputers[14].szName = rm.GetString("IDS_UC_DVD");
            _sComputers[14].szUnits = rm.GetString("IDS_UC_DVD");
            _sComputers[15].szName = rm.GetString("IDS_UC_DVDDL");
            _sComputers[15].szUnits = rm.GetString("IDS_UC_DVDDL_2");
            _sComputers[16].szName = rm.GetString("IDS_UC_DVDR");
            _sComputers[16].szUnits = rm.GetString("IDS_UC_DVDR");
            //end computers//////////////////////////////////////////////////////////////////////////////////////

            //Area Conversion Intialization//////////////////////////////////////////////////////////////////////
            _sArea = new TypeUnitConverter[NUMBER_OF_AREA];
            for (int i = 0; i < NUMBER_OF_AREA; i++) _sArea[i] = new TypeUnitConverter();
            _sArea[0].szName = rm.GetString("IDS_UA_Acres");
            _sArea[0].szUnits = rm.GetString("IDS_UA_A");
            _sArea[1].szName = rm.GetString("IDS_UA_Hectares");
            _sArea[1].szUnits = rm.GetString("IDS_UA_ha");
            _sArea[2].szName = rm.GetString("IDS_UA_Square_Kilometer");
            _sArea[2].szUnits = rm.GetString("IDS_UA_km");
            _sArea[3].szName = rm.GetString("IDS_UA_Square_Meters");
            _sArea[3].szUnits = rm.GetString("IDS_UA_m");
            _sArea[4].szName = rm.GetString("IDS_UA_Square_Centimeters");
            _sArea[4].szUnits = rm.GetString("IDS_UA_cm");
            _sArea[5].szName = rm.GetString("IDS_UA_Square_Millimeters");
            _sArea[5].szUnits = rm.GetString("IDS_UA_mm");
            _sArea[6].szName = rm.GetString("IDS_UA_Square_Miles");
            _sArea[6].szUnits = rm.GetString("IDS_UA_mi");
            _sArea[7].szName = rm.GetString("IDS_UA_Square_Yards");
            _sArea[7].szUnits = rm.GetString("IDS_UA_yd");
            _sArea[8].szName = rm.GetString("IDS_UA_Square_Feet");
            _sArea[8].szUnits = rm.GetString("IDS_UA_ft");
            _sArea[9].szName = rm.GetString("IDS_UA_Square_Inches");
            _sArea[9].szUnits = rm.GetString("IDS_UA_in");
            _sArea[10].szName = rm.GetString("IDS_UA_Square_MilesS");
            _sArea[10].szUnits = rm.GetString("IDS_UA_miS");
            _sArea[11].szName = rm.GetString("IDS_UA_Square_FeetS");
            _sArea[11].szUnits = rm.GetString("IDS_UA_ftS");
            _sArea[12].szName = rm.GetString("IDS_UA_Square_InchesS");
            _sArea[12].szUnits = rm.GetString("IDS_UA_inS");
            //end Area//////////////////////////////////////////////////////////////////////////////////////////

            //Begin Energy Initialization///////////////////////////////////////////////////////////////////////
            _sEnergy = new TypeUnitConverter[NUMBER_OF_ENERGY];
            for (int i = 0; i < NUMBER_OF_ENERGY; i++) _sEnergy[i] = new TypeUnitConverter();
            _sEnergy[0].szName = rm.GetString("IDS_UE_British_thermal_units");
            _sEnergy[0].szUnits = rm.GetString("IDS_UE_Btu");
            _sEnergy[1].szName = rm.GetString("IDS_UE_Calories");
            _sEnergy[1].szUnits = rm.GetString("IDS_UE_cal");
            _sEnergy[2].szName = rm.GetString("IDS_UE_Foot_Pounds");
            _sEnergy[2].szUnits = rm.GetString("IDS_UE_Ft_lbf");
            _sEnergy[3].szName = rm.GetString("IDS_UE_HorsePower_per_Hour");
            _sEnergy[3].szUnits = rm.GetString("IDS_UE_Hp_hr");
            _sEnergy[4].szName = rm.GetString("IDS_UE_Joules");
            _sEnergy[4].szUnits = rm.GetString("IDS_UE_J");
            _sEnergy[5].szName = rm.GetString("IDS_UE_Kilocalories");
            _sEnergy[5].szUnits = rm.GetString("IDS_UE_kcal");
            _sEnergy[6].szName = rm.GetString("IDS_UE_Kilogram_Meters");
            _sEnergy[6].szUnits = rm.GetString("IDS_UE_Kg_m");
            _sEnergy[7].szName = rm.GetString("IDS_UE_KJoules");
            _sEnergy[7].szUnits = rm.GetString("IDS_UE_kJ");
            _sEnergy[8].szName = rm.GetString("IDS_UE_Kilowatt_hours");
            _sEnergy[8].szUnits = rm.GetString("IDS_UE_kW_h");
            _sEnergy[9].szName = rm.GetString("IDS_UE_Therms");
            _sEnergy[9].szUnits = rm.GetString("IDS_UE_thm");
            _sEnergy[10].szName = rm.GetString("IDS_UE_Watt_hours");
            _sEnergy[10].szUnits = rm.GetString("IDS_UE_W_h");
            _sEnergy[11].szName = rm.GetString("IDS_UE_Watt_seconds");
            _sEnergy[11].szUnits = rm.GetString("IDS_UE_W_s");
            _sEnergy[12].szName = rm.GetString("IDS_UE_Erg");
            _sEnergy[12].szUnits = rm.GetString("IDS_UE_Erg2");
            //end energy///////////////////////////////////////////////////////////////////////////////////////////


            //Begin Power Intitialize////////////////////////////////////////////////////////////////////////////////
            _sPower = new TypeUnitConverter[NUMBER_OF_POWER];
            for (int i = 0; i < NUMBER_OF_POWER; i++) _sPower[i] = new TypeUnitConverter();
            _sPower[0].szName = rm.GetString("IDS_UP_Terrawatts");
            _sPower[0].szUnits = rm.GetString("IDS_UP_TW");
            _sPower[1].szName = rm.GetString("IDS_UP_Gigawatts");
            _sPower[1].szUnits = rm.GetString("IDS_UP_GW");
            _sPower[2].szName = rm.GetString("IDS_UP_MegaWatts");
            _sPower[2].szUnits = rm.GetString("IDS_UP_MW");
            _sPower[3].szName = rm.GetString("IDS_UP_Kilowatts");
            _sPower[3].szUnits = rm.GetString("IDS_UP_kW");
            _sPower[4].szName = rm.GetString("IDS_UP_Watts");
            _sPower[4].szUnits = rm.GetString("IDS_UP_W");
            _sPower[5].szName = rm.GetString("IDS_UP_Milliwatts");
            _sPower[5].szUnits = rm.GetString("IDS_UP_milW");
            _sPower[6].szName = rm.GetString("IDS_UP_Horse_Power_metric");
            _sPower[6].szUnits = rm.GetString("IDS_UP_hp_metric");
            _sPower[7].szName = rm.GetString("IDS_UP_Horse_PowerIT");
            _sPower[7].szUnits = rm.GetString("IDS_UP_hpIT");
            _sPower[8].szName = rm.GetString("IDS_UP_Calories_per_second");
            _sPower[8].szUnits = rm.GetString("IDS_UP_cal_s");
            _sPower[9].szName = rm.GetString("IDS_UP_Calories_per_min");
            _sPower[9].szUnits = rm.GetString("IDS_UP_cal_min");
            _sPower[10].szName = rm.GetString("IDS_UP_Calories_per_hour");
            _sPower[10].szUnits = rm.GetString("IDS_UP_cal_h");
            _sPower[11].szName = rm.GetString("IDS_UP_Btu_per_second");
            _sPower[11].szUnits = rm.GetString("IDS_UP_Btu_s");
            _sPower[12].szName = rm.GetString("IDS_UP_Btu_per_minute");
            _sPower[12].szUnits = rm.GetString("IDS_UP_Btu_min");
            _sPower[13].szName = rm.GetString("IDS_UP_Btu_per_hour");
            _sPower[13].szUnits = rm.GetString("IDS_UP_Btu_h");
            _sPower[14].szName = rm.GetString("IDS_UP_Foot_Pounds_per_sec");
            _sPower[14].szUnits = rm.GetString("IDS_UP_ft_lb_s");
            _sPower[15].szName = rm.GetString("IDS_UP_Foot_Pounds_per_min");
            _sPower[15].szUnits = rm.GetString("IDS_UP_ft_lb_min");
            _sPower[16].szName = rm.GetString("IDS_UP_Foot_Pounds_per_hour");
            _sPower[16].szUnits = rm.GetString("IDS_UP_ft_lb_hr");
            _sPower[17].szName = rm.GetString("IDS_UP_Tons_Refrigeration");
            _sPower[17].szUnits = rm.GetString("IDS_UP_TR");
            //end Power//////////////////////////////////////////////////////////////////////////////////////////


            // Begin Mass Intialize//////////////////////////////////////////////////////////////////////////////
            _sMass = new TypeUnitConverter[NUMBER_OF_MASS];
            for (int i = 0; i < NUMBER_OF_MASS; i++) _sMass[i] = new TypeUnitConverter();
            _sMass[0].szName = rm.GetString("IDS_UM_Grams");
            _sMass[0].szUnits = rm.GetString("IDS_UM_g");
            _sMass[1].szName = rm.GetString("IDS_UM_Kilograms");
            _sMass[1].szUnits = rm.GetString("IDS_UM_kg");
            _sMass[2].szName = rm.GetString("IDS_UM_Ounces");
            _sMass[2].szUnits = rm.GetString("IDS_UM_oz");
            _sMass[3].szName = rm.GetString("IDS_UM_OuncesTroy");
            _sMass[3].szUnits = rm.GetString("IDS_UM_ozTroy");
            _sMass[4].szName = rm.GetString("IDS_UM_Pounds");
            _sMass[4].szUnits = rm.GetString("IDS_UM_lb");
            _sMass[5].szName = rm.GetString("IDS_UM_PoundsTroy");
            _sMass[5].szUnits = rm.GetString("IDS_UM_lbTroy");
            _sMass[6].szName = rm.GetString("IDS_UM_Tonnes_metric");
            _sMass[6].szUnits = rm.GetString("IDS_UM_ts");
            _sMass[7].szName = rm.GetString("IDS_UM_Tons_US_short");
            _sMass[7].szUnits = rm.GetString("IDS_UM_tn");
            _sMass[8].szName = rm.GetString("IDS_UM_Tons_UK_long");
            _sMass[8].szUnits = rm.GetString("IDS_UM_T");
            _sMass[9].szName = rm.GetString("IDS_UM_Stones");
            _sMass[9].szUnits = rm.GetString("IDS_UM_st");
            _sMass[10].szName = rm.GetString("IDS_UM_Hunweights_US_short");
            _sMass[10].szUnits = rm.GetString("IDS_UM_Cwt_US");
            _sMass[11].szName = rm.GetString("IDS_UM_Hunweights_UK_long");
            _sMass[11].szUnits = rm.GetString("IDS_UM_Cwt_UK");
            //end Mass///////////////////////////////////////////////////////////////////////////////////////////

            //Begin Force Initialize/////////////////////////////////////////////////////////////////////////////
            _sForce = new TypeUnitConverter[NUMBER_OF_FORCE];
            for (int i = 0; i < NUMBER_OF_FORCE; i++) _sForce[i] = new TypeUnitConverter();
            _sForce[0].szName = rm.GetString("IDS_UF_Dynes");
            _sForce[0].szUnits = rm.GetString("IDS_UF_d");
            _sForce[1].szName = rm.GetString("IDS_UF_gram_Force");
            _sForce[1].szUnits = rm.GetString("IDS_UF_gf");
            _sForce[2].szName = rm.GetString("IDS_UF_Kilgram_Force");
            _sForce[2].szUnits = rm.GetString("IDS_UF_kgf");
            _sForce[3].szName = rm.GetString("IDS_UF_KiloNewtons");
            _sForce[3].szUnits = rm.GetString("IDS_UF_kN");
            _sForce[4].szName = rm.GetString("IDS_UF_MegaNewtons");
            _sForce[4].szUnits = rm.GetString("IDS_UF_MN");
            _sForce[5].szName = rm.GetString("IDS_UF_Newtons");
            _sForce[5].szUnits = rm.GetString("IDS_UF_N");
            _sForce[6].szName = rm.GetString("IDS_UF_Ounce_Force");
            _sForce[6].szUnits = rm.GetString("IDS_UF_of");
            _sForce[7].szName = rm.GetString("IDS_UF_Pound_force");
            _sForce[7].szUnits = rm.GetString("IDS_UF_lbf");
            _sForce[8].szName = rm.GetString("IDS_UF_Tonnes_force_metric");
            _sForce[8].szUnits = rm.GetString("IDS_UF_tnf_metric");
            _sForce[9].szName = rm.GetString("IDS_UF_Tons_force_US");
            _sForce[9].szUnits = rm.GetString("IDS_UF_tnf_US");
            _sForce[10].szName = rm.GetString("IDS_UF_Tons_force_UK");
            _sForce[10].szUnits = rm.GetString("IDS_UF_tnf_UK");
            _sForce[11].szName = rm.GetString("IDS_UF_KipForce");
            _sForce[11].szUnits = rm.GetString("IDS_UF_kip");
            //end Force//////////////////////////////////////////////////////////////////////////////////////////////

            //Begin Pressure Initialization
            _sPressure = new TypeUnitConverter[NUMBER_OF_PRESSURE];
            for (int i = 0; i < NUMBER_OF_PRESSURE; i++) _sPressure[i] = new TypeUnitConverter();
            _sPressure[0].szName = rm.GetString("IDS_UP_Kilogram_force_per_meter");
            _sPressure[0].szUnits = rm.GetString("IDS_UP_kg_m");
            _sPressure[1].szName = rm.GetString("IDS_UP_Kilogram_force_per_cm");
            _sPressure[1].szUnits = rm.GetString("IDS_UP_kg_cm");
            _sPressure[2].szName = rm.GetString("IDS_UP_Kilogram_force_per_mm");
            _sPressure[2].szUnits = rm.GetString("IDS_UP_kg_mm");
            _sPressure[3].szName = rm.GetString("IDS_UP_Newtons_per_meter");
            _sPressure[3].szUnits = rm.GetString("IDS_UP_N_m");
            _sPressure[4].szName = rm.GetString("IDS_UP_Newtons_per_cm");
            _sPressure[4].szUnits = rm.GetString("IDS_UP_N_cm");
            _sPressure[5].szName = rm.GetString("IDS_UP_Newtons_per_mm");
            _sPressure[5].szUnits = rm.GetString("IDS_UP_N_mm");
            _sPressure[6].szName = rm.GetString("IDS_UP_Bars");
            _sPressure[6].szUnits = rm.GetString("IDS_UP_b");
            _sPressure[7].szName = rm.GetString("IDS_UP_Millibars");
            _sPressure[7].szUnits = rm.GetString("IDS_UP_mb");
            _sPressure[8].szName = rm.GetString("IDS_UP_Pascals");
            _sPressure[8].szUnits = rm.GetString("IDS_UP_Pa");
            _sPressure[9].szName = rm.GetString("IDS_UP_Hectopascals");
            _sPressure[9].szUnits = rm.GetString("IDS_UP_hPa");
            _sPressure[10].szName = rm.GetString("IDS_UP_Atmospheres");
            _sPressure[10].szUnits = rm.GetString("IDS_UP_atm");
            _sPressure[11].szName = rm.GetString("IDS_UP_Torrs");
            _sPressure[11].szUnits = rm.GetString("IDS_UP_mm_Hg_0");
            _sPressure[12].szName = rm.GetString("IDS_UP_Inches_Merc");
            _sPressure[12].szUnits = rm.GetString("IDS_UP_Hg");
            _sPressure[13].szName = rm.GetString("IDS_UP_Pounds_force_per_foot");
            _sPressure[13].szUnits = rm.GetString("IDS_UP_lb_ft");
            _sPressure[14].szName = rm.GetString("IDS_UP_Pounds_force_per_inch");
            _sPressure[14].szUnits = rm.GetString("IDS_UP_PSI");
            _sPressure[15].szName = rm.GetString("IDS_UP_Tons_US_per_foot");
            _sPressure[15].szUnits = rm.GetString("IDS_UP_tn_ft");
            _sPressure[16].szName = rm.GetString("IDS_UP_Tons_US_per_inch");
            _sPressure[16].szUnits = rm.GetString("IDS_UP_tn_in");
            _sPressure[17].szName = rm.GetString("IDS_UP_Tons_UK_per_foot");
            _sPressure[17].szUnits = rm.GetString("IDS_UP_T_ft");
            _sPressure[18].szName = rm.GetString("IDS_UP_Tons_UK_per_inch");
            _sPressure[18].szUnits = rm.GetString("IDS_UP_T_in");
            _sPressure[19].szName = rm.GetString("IDS_UP_Tonnes_per_meter");
            _sPressure[19].szUnits = rm.GetString("IDS_UP_t_m");
            _sPressure[20].szName = rm.GetString("IDS_UP_Tonnes_per_cm");
            _sPressure[20].szUnits = rm.GetString("IDS_UP_t_cm");
            //end Pressure////////////////////////////////////////////////////////////////////////////////////////


            //Begin Fuel Econ
            _sFuelEcon = new TypeUnitConverter[NUMBER_OF_FUELECON];
            for (int i = 0; i < NUMBER_OF_FUELECON; i++) _sFuelEcon[i] = new TypeUnitConverter();
            _sFuelEcon[0].szName = rm.GetString("IDS_UF_Liters100km");
            _sFuelEcon[0].szUnits = rm.GetString("IDS_UF_l100km");
            _sFuelEcon[1].szName = rm.GetString("IDS_UF_Miles_Gallon");
            _sFuelEcon[1].szUnits = rm.GetString("IDS_UF_milesg");
            _sFuelEcon[2].szName = rm.GetString("IDS_UF_Miles_Gallon_UK");
            _sFuelEcon[2].szUnits = rm.GetString("IDS_UF_milesg_UK");
            _sFuelEcon[3].szName = rm.GetString("IDS_UF_Kilmeters_Gallon");
            _sFuelEcon[3].szUnits = rm.GetString("IDS_UF_kmg");
            _sFuelEcon[4].szName = rm.GetString("IDS_UF_Kilimeters_Gallon_UK");
            _sFuelEcon[4].szUnits = rm.GetString("IDS_UF_kmg_UK");
            _sFuelEcon[5].szName = rm.GetString("IDS_UF_Miles_Liter");
            _sFuelEcon[5].szUnits = rm.GetString("IDS_UF_milesl");
            _sFuelEcon[6].szName = rm.GetString("IDS_UF_Kilmeters_Liter");
            _sFuelEcon[6].szUnits = rm.GetString("IDS_UF_kml");
            _sFuelEcon[7].szName = rm.GetString("IDS_UF_Gallons_100miles");
            _sFuelEcon[7].szUnits = rm.GetString("IDS_UF_g100miles");
            _sFuelEcon[8].szName = rm.GetString("IDS_UF_Gallons_100km");
            _sFuelEcon[8].szUnits = rm.GetString("IDS_UF_g100km");
            _sFuelEcon[9].szName = rm.GetString("IDS_UF_Gallons_100miles_UK");
            _sFuelEcon[9].szUnits = rm.GetString("IDS_UF_g100miles_UK");
            _sFuelEcon[10].szName = rm.GetString("IDS_UF_Gallons_100kilmeters_UK");
            _sFuelEcon[10].szUnits = rm.GetString("IDS_UF_g100km_UK");
            _sFuelEcon[11].szName = rm.GetString("IDS_UF_Liters_100miles");
            _sFuelEcon[11].szUnits = rm.GetString("IDS_UF_l100miles");
            //End Fuel Econ

            _sTime = new TypeUnitConverter[NUMBER_OF_TIME];
            for (int i = 0; i < NUMBER_OF_TIME; i++) _sTime[i] = new TypeUnitConverter();
            _sTime[0].szName = rm.GetString("IDS_UT_NanoSec");
            _sTime[0].szUnits = rm.GetString("IDS_UT_ns");
            _sTime[1].szName = rm.GetString("IDS_UT_MilliSec");
            _sTime[1].szUnits = rm.GetString("IDS_UT_ms");
            _sTime[2].szName = rm.GetString("IDS_UT_Sec");
            _sTime[2].szUnits = rm.GetString("IDS_UT_s");
            _sTime[3].szName = rm.GetString("IDS_UT_Minute");
            _sTime[3].szUnits = rm.GetString("IDS_UT_min");
            _sTime[4].szName = rm.GetString("IDS_UT_Hour");
            _sTime[4].szUnits = rm.GetString("IDS_UT_h");
            _sTime[5].szName = rm.GetString("IDS_UT_Days");
            _sTime[5].szUnits = rm.GetString("IDS_UT_d");
            _sTime[6].szName = rm.GetString("IDS_UT_WorkWeeks");
            _sTime[6].szUnits = rm.GetString("IDS_UT_ww");
            _sTime[7].szName = rm.GetString("IDS_UT_Weeks");
            _sTime[7].szUnits = rm.GetString("IDS_UT_wks");
            _sTime[8].szName = rm.GetString("IDS_UT_Months");
            _sTime[8].szUnits = rm.GetString("IDS_UT_m");
            _sTime[9].szName = rm.GetString("IDS_UT_Years");
            _sTime[9].szUnits = rm.GetString("IDS_UT_yrs");
            _sTime[10].szName = rm.GetString("IDS_UT_Decades");
            _sTime[10].szUnits = rm.GetString("IDS_UT_dec");
            _sTime[11].szName = rm.GetString("IDS_UT_Centuries");
            _sTime[11].szUnits = rm.GetString("IDS_UT_cent");
            _sTime[12].szName = rm.GetString("IDS_UT_Milleniums");
            _sTime[12].szUnits = rm.GetString("IDS_UT_millen");
            //End Time

            return true;
        }
        public int GetSectionCount()
        {
            return (int)ConvertionSectionType.CONVERT_Count;
        }
        public void UpdateItems()
        {
            NotifyPropertyChanged("szFrom");
            NotifyPropertyChanged("szTo");
            NotifyPropertyChanged("szFromUnits");
            NotifyPropertyChanged("szToUnits");
            NotifyPropertyChanged("szFromName");
            NotifyPropertyChanged("szToName");
        }

        public bool SetSubSectionFrom(int iIndex)
        {
            if (iIndex < 0)
                return false;

            _sSections[_iCurrentSection].btFrom = (byte)iIndex;

            UpdateItems();

            return true;
        }
        public bool SetSubSectionTo(int iIndex)
        {
            if (iIndex < 0)
                return false;

            _sSections[_iCurrentSection].btTo = (byte)iIndex;

            UpdateItems();

            return true;
        }
        public bool SetSection(int iIndex)
        {
            if(iIndex < 0 || iIndex >= GetSectionCount())
                return false;

            _iCurrentSection = iIndex;

            NotifyPropertyChanged("szSection");
            UpdateItems();

            return true;
        }
        public string GetSectionName(int iIndex)
        {
            return _sSections[iIndex].szName;
        }

        public string GetSectionImage(int iIndex)
        {
            return _sSections[iIndex].szImage;
        }

        public int GetSubSectionCount()
        {
            switch (_iCurrentSection)
            {
                case (int)ConvertionSectionType.CONVERT_LENGTH:
                    return NUMBER_OF_LENGTH;
                case (int)ConvertionSectionType.CONVERT_TEMPERATURE:
                    return NUMBER_OF_TEMP;
                case (int)ConvertionSectionType.CONVERT_SPEED:
                    return NUMBER_OF_SPEED;
                case (int)ConvertionSectionType.CONVERT_COMPUTER:
                    return NUMBER_OF_COMP;
                case (int)ConvertionSectionType.CONVERT_AREA:
                    return NUMBER_OF_AREA;
                case (int)ConvertionSectionType.CONVERT_ENERGY:
                    return NUMBER_OF_ENERGY;
                case (int)ConvertionSectionType.CONVERT_POWER:
                    return NUMBER_OF_POWER;
                case (int)ConvertionSectionType.CONVERT_FORCE:
                    return NUMBER_OF_FORCE;
                case (int)ConvertionSectionType.CONVERT_MASS:
                    return NUMBER_OF_MASS;
                case (int)ConvertionSectionType.CONVERT_PRESSURE:
                    return NUMBER_OF_PRESSURE;
                case (int)ConvertionSectionType.CONVERT_COOKING:
                    return NUMBER_OF_COOKING;
                case (int)ConvertionSectionType.CONVERT_VOLUME:
                    return NUMBER_OF_VOLUME;
                case (int)ConvertionSectionType.CONVERT_FUEL_ECON:
                    return NUMBER_OF_FUELECON;
                case (int)ConvertionSectionType.CONVERT_TIME:
                    return NUMBER_OF_TIME;
                case (int)ConvertionSectionType.CONVERT_ANGLES:
                    return NUMBER_OF_ANGLES;
                case (int)ConvertionSectionType.CONVERT_TORQUE:
                    return NUMBER_OF_TORQUE;
                case (int)ConvertionSectionType.CONVERT_DENSITY:
                    return NUMBER_OF_DENSITY;
                case (int)ConvertionSectionType.CONVERT_ACCELERATION:
                    return NUMBER_OF_ACCELERATION;
                default:
                    return 0;
            }
        }
        public string GetSubSectionName(int iIndex)
        {
            switch (_iCurrentSection)
            {
                case (int)ConvertionSectionType.CONVERT_LENGTH:
                    return _sLength[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_TEMPERATURE:
                    return _sTemp[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_SPEED:
                    return _sSpeed[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_COMPUTER:
                    return _sComputers[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_AREA:
                    return _sArea[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_ENERGY:
                    return _sEnergy[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_POWER:
                    return _sPower[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_FORCE:
                    return _sForce[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_MASS:
                    return _sMass[iIndex].szName;
                 case (int)ConvertionSectionType.CONVERT_PRESSURE:
                    return _sPressure[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_COOKING:
                    return _sCooking[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_VOLUME:
                    return _sVolume[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_FUEL_ECON:
                    return _sFuelEcon[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_TIME:
                    return _sTime[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_ANGLES:
                    return _sAngles[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_TORQUE:
                    return _sTorque[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_DENSITY:
                    return _sDensity[iIndex].szName;
                case (int)ConvertionSectionType.CONVERT_ACCELERATION:
                    return _sAcceleration[iIndex].szName;
                default:
                    return "";
            }
        }
        public string GetSubSectionUnits(int iIndex)
        {
            switch (_iCurrentSection)
            {
                case (int)ConvertionSectionType.CONVERT_LENGTH:
                    return _sLength[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_TEMPERATURE:
                    return _sTemp[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_SPEED:
                    return _sSpeed[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_COMPUTER:
                    return _sComputers[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_AREA:
                    return _sArea[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_ENERGY:
                    return _sEnergy[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_POWER:
                    return _sPower[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_FORCE:
                    return _sForce[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_MASS:
                    return _sMass[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_PRESSURE:
                    return _sPressure[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_COOKING:
                    return _sCooking[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_VOLUME:
                    return _sVolume[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_FUEL_ECON:
                    return _sFuelEcon[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_TIME:
                    return _sTime[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_ANGLES:
                    return _sAngles[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_TORQUE:
                    return _sTorque[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_DENSITY:
                    return _sDensity[iIndex].szUnits;
                case (int)ConvertionSectionType.CONVERT_ACCELERATION:
                    return _sAcceleration[iIndex].szUnits;
                default:
                    return "";
            }
        }
        public string GetResult(string szInput)
        {
            if (szInput == "")
                return szInput;

            double dbIn;
            dbOut = 0.00;

            int iFrom = (int)_sSections[_iCurrentSection].btFrom;
            int iTo = (int)_sSections[_iCurrentSection].btTo;

            //dbIn = Convert.ToDouble(szInput);
            bool bConv = Double.TryParse(szInput, out dbIn);

            switch (_iCurrentSection)
            {
                case (int)ConvertionSectionType.CONVERT_LENGTH:
                    dbOut = dbIn / _dbLength[iFrom] * _dbLength[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_TEMPERATURE:
                    dbOut = ConvertTemp(dbIn);
                    break;
                case (int)ConvertionSectionType.CONVERT_SPEED:
                    dbOut = dbIn / _dbSpeed[iFrom] * _dbSpeed[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_COMPUTER:
                    dbOut = dbIn * _dbComputers[iFrom] / _dbComputers[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_AREA:
                    dbOut = dbIn * _dbArea[iFrom] / _dbArea[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_ENERGY:                    
                    dbOut = dbIn / _dbEnergy[iFrom] * _dbEnergy[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_POWER:
                    dbOut = dbIn / _dbPower[iFrom] * _dbPower[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_FORCE:
                    dbOut = dbIn / _dbForce[iFrom] * _dbForce[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_MASS:
                    dbOut = dbIn / _dbMass[iFrom] * _dbMass[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_PRESSURE:
                    dbOut = dbIn / _dbPressure[iFrom] * _dbPressure[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_COOKING:
                    dbOut = dbIn / _dbCooking[iFrom] * _dbCooking[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_VOLUME:
                    dbOut = dbIn / _dbVolume[iFrom] * _dbVolume[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_FUEL_ECON:
                    dbOut = ConvertFuelEcon(dbIn);
                    break;
                case (int)ConvertionSectionType.CONVERT_TIME:
                    dbOut = dbIn / _dbTime[iFrom] * _dbTime[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_ANGLES:
                    dbOut = dbIn / _dbAngles[iFrom] * _dbAngles[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_TORQUE:
                    dbOut = dbIn / _dbTorque[iFrom] * _dbTorque[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_DENSITY:
                    dbOut = dbIn / _dbDensity[iFrom] * _dbDensity[iTo];
                    break;
                case (int)ConvertionSectionType.CONVERT_ACCELERATION:
                    dbOut = dbIn / _dbAcceleration[iFrom] * _dbAcceleration[iTo];
                    break;
                default:
                    dbOut = 0;
                    break;
            }

            return Convert.ToString(dbOut);
        }

        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this,
                    new PropertyChangedEventArgs(propertyName));
            }

        }
        private double ConvertTemp(double dbIn)
        {
            int iFrom = (int)_sSections[_iCurrentSection].btFrom;
            int iTo = (int)_sSections[_iCurrentSection].btTo;

	        if(iFrom == (int)TemperatureType.TEMP_CELSIUS)
	        {
		        if(dbIn < -273.15)
			        dbIn = -273.15;
		        if(iTo == (int)TemperatureType.TEMP_KELVIN)
			        return dbIn + 273.15;
		        else if(iTo == (int)TemperatureType.TEMP_FAHRENHEIT)
			        return dbIn*1.8 + 32;
		        else if(iTo == (int)TemperatureType.TEMP_RANKINE)
			        return (dbIn + 273.15)*1.8;
                else if(iTo == (int)TemperatureType.TEMP_REAUMUR)
                    return (dbIn*0.8);
	        }

	        else if(iFrom == (int)TemperatureType.TEMP_KELVIN)
	        {
		        if(dbIn<0)
			        dbIn=0;
		        if(iTo == (int)TemperatureType.TEMP_CELSIUS)
			        return dbIn-273.15;
		        else if(iTo == (int)TemperatureType.TEMP_FAHRENHEIT)
			        return 1.8*dbIn-459.67;
		        else if(iTo == (int)TemperatureType.TEMP_RANKINE)
			        return  dbIn*1.8;
                else if(iTo == (int)TemperatureType.TEMP_REAUMUR)
                    return (dbIn - 273.15)*0.8;
	        }

	        else if(iFrom == (int)TemperatureType.TEMP_FAHRENHEIT)
	        {
		        if(dbIn<-459.67)
			        dbIn=-459.67;
		        if(iTo == (int)TemperatureType.TEMP_CELSIUS)
			        return (dbIn-32.0)/1.8;
		        else if(iTo == (int)TemperatureType.TEMP_KELVIN)
			        return (dbIn-32.0)/1.8+273.15;
		        else if(iTo == (int)TemperatureType.TEMP_RANKINE)
			        return dbIn + 459.67;
                else if(iTo == (int)TemperatureType.TEMP_REAUMUR)
                    return (dbIn - 32)*4.0/9.0;
	        }

	        else if(iFrom == (int)TemperatureType.TEMP_RANKINE)
	        {
		        if(dbIn<0)
			        dbIn=0;
		        if(iTo == (int)TemperatureType.TEMP_CELSIUS)
			        return (dbIn - 491.67)/1.8;
		        else if(iTo == (int)TemperatureType.TEMP_KELVIN)
			        return dbIn/1.8;
		        else if(iTo == (int)TemperatureType.TEMP_FAHRENHEIT)
			        return dbIn - 459.67;
                else if(iTo == (int)TemperatureType.TEMP_REAUMUR)
                    return (dbIn - 491.67)*4.0/9.0;
	        }
            else if(iFrom == (int)TemperatureType.TEMP_REAUMUR)
            {
                if(dbIn< -218.52)
                    dbIn = -218.52;
                if(iTo == (int)TemperatureType.TEMP_CELSIUS)
                    return dbIn*5.0/4.0;
                else if(iTo == (int)TemperatureType.TEMP_KELVIN)
                    return dbIn*5.0/5.0 + 273.15;
                else if(iTo == (int)TemperatureType.TEMP_FAHRENHEIT)
                    return dbIn*9.0/4.0 + 32;
                else if(iTo == (int)TemperatureType.TEMP_RANKINE)
                    return dbIn*9.0/4.0 + 491.67;
            }


	        //if we fall through everything return the same value
	        return dbIn;
        }
        private double ConvertFuelEcon(double dbIn)
        {
            //so ... we have two "types" of economy
            //volume/distance & distance/volume
            //so we'll have to do some math to make this happen
            int iFrom = (int)_sSections[_iCurrentSection].btFrom;
            int iTo = (int)_sSections[_iCurrentSection].btTo;

            if(dbIn == 0)
                return 0;

            //if we're positive its the same type of conversion so just go
            if(_dbFuelEcon[iFrom]*_dbFuelEcon[iTo] > 0)
                return dbIn/_dbFuelEcon[iFrom]*_dbFuelEcon[iTo];    

            else //we're inversely proportional so
               //don't forget to switch the sign
                return -1.0*_dbFuelEcon[iFrom]/dbIn*_dbFuelEcon[iTo];
        }

        public void Switch()
        {
            //units
            int iTemp = iCurrentFrom;

            string szTemp = dbOut.ToString();

            SetSubSectionFrom(iCurrentTo);
            SetSubSectionTo(iTemp);
    
            //values
            
            if (szTemp != null)
            {
                Globals.Calc.AddString(szTemp);
                szTemp = szTo;
            }
        }
    }
}
