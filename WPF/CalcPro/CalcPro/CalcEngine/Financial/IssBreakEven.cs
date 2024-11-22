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

namespace CalcPro
{
    public class IssBreakEven
    {
        public IssBreakEven()
        {
            Clear();
        }

        double dbFixedCost;			//enter/compute
        double dbVariableCostPer;	//enter/compute
        double dbUnitPrice;			//enter/compute
        double dbProfit;			//enter/compute
        double dbQuantity;			//enter/compute

        /*	*	*	*	*	*	*	*	*	*	*	*	*	*	
	    Breakeven

	    PFT =	 P*Q - (FC + VC * Q)

	    where: 
	    PFT		= profit
	    P		= price
	    FC		= fixed cost
	    VC		= variable cost
	    Q		= quantity
        *	*	*	*	*	*	*	*	*	*	*	*	*	*/

        public void Clear()
        {
            dbFixedCost = 0.0;
            dbProfit = 0.0;
            dbQuantity = 0.0;
            dbVariableCostPer = 0.0;
            dbUnitPrice = 0.0;
        }

       	public void			SetFixedCost(double dbFix){dbFixedCost = dbFix;}
        public void			SetVariableCostPer(double dbCost){dbVariableCostPer = dbCost;}
        public void			SetUnitPrice(double dbUnit){dbUnitPrice = dbUnit;}
        public void			SetProfit(double dbNewProfit){dbProfit = dbNewProfit;}
        public void			SetQuantity(double dbNewQuantity){dbQuantity = dbNewQuantity;}

        public double		GetFixedCost(){return dbFixedCost;}
        public double		GetVariableCost(){return dbVariableCostPer;}
        public double		GetUnitPrice(){return dbUnitPrice;}
        public double		GetProfit(){return dbProfit;}
        public double		GetQuantity(){return dbQuantity;}

        public double CalcProfit()
        {
	        return dbProfit			= dbUnitPrice * dbQuantity - dbFixedCost - dbVariableCostPer * dbQuantity;
        }

        public double CalcFixedCost()
        {
	        return dbFixedCost		= dbUnitPrice * dbQuantity - dbVariableCostPer * dbQuantity - dbProfit;	
        }


        public double CalcQuantity()
        {
	        return dbQuantity		= (dbProfit + dbFixedCost) / (dbUnitPrice - dbVariableCostPer);
        }

        public double CalcUnitPrice()
        {
	        return dbUnitPrice		= (dbProfit + dbFixedCost + dbVariableCostPer*dbQuantity) / dbQuantity;
        }

        public double CalcVariableCostPer()
        {
	        return dbVariableCostPer	= (dbUnitPrice * dbQuantity - dbFixedCost - dbProfit) / dbQuantity;
        }
    }
}
