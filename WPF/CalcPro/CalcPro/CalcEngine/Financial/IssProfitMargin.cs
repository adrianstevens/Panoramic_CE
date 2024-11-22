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
    public class IssProfitMargin
    {
        public IssProfitMargin()
        {
            Clear();
        }

        double dbCost;		//enter/compute
        double dbSellPrice;	//enter/compute
        double dbProfitMar;	//enter/compute

        public void				SetCost(double dbCostIn){dbCost = dbCostIn;}
        public void SetSellPrice(double dbSell) { dbSellPrice = dbSell; }
        public void SetProfitMargin(double dbProf) { dbProfitMar = dbProf; }

        public double GetCost() { return dbCost; }
        public double GetSellPrice() { return dbSellPrice; }
        public double GetProfitMargin() { return dbProfitMar; }

        public void Clear()
        {
	        dbCost		= 0.0;
	        dbProfitMar	= 0.0;
	        dbSellPrice	= 0.0;
        }


        /********************************************************************
	        Profit Margin

	        Gross Profit Margin = Selling Price - Cost / Selling Price * 100
        ********************************************************************/
        public double CalcProfit()
        {
	        return dbProfitMar	= (dbSellPrice - dbCost) * 100.0 / dbSellPrice;
        }

        public double CalcCost()
        {
	        return dbCost		= dbSellPrice - dbSellPrice*dbProfitMar / 100.0;
        }

        public double CalcSellPrice()
        {
	        return dbSellPrice	= 100.0 * dbCost / ( 100.0 - dbProfitMar);
        }
    }
}
