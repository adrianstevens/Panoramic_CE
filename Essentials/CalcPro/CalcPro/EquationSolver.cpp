#include "EquationSolver.h"

//this pair should work at least .... might be able to bump up both
//#define SOLVER_DECIMAL_PRECISION    0.00000000001
//#define DYDX_DELTA                  0.00000001

#define SOLVER_DECIMAL_PRECISION    0.00000000001
#define DYDX_DELTA                  0.00000001
#define MAX_SOLVE_LOOPS             1000
#define MIN_SOLVER_RANGE 5.0

CEquationSolver::CEquationSolver(void)
{
    m_oCalc     = SciCalc::Instance();
    m_oStr      = CIssString::Instance();
    m_oUtil     = CIssSciCalcUtil::Instance();

#ifdef DEBUG
    m_oStr->Empty(m_szEquation);
    m_oStr->Empty(m_szGuess);
    m_oStr->Empty(m_szAnswer);

    SolveEquation();
#endif

}

CEquationSolver::~CEquationSolver(void)
{
}

HRESULT CEquationSolver::SolveEquation()
{
    HRESULT hr = S_OK;
    double  dbAns, dbGuess;
//    TCHAR   szResult[STRING_NORMAL];

    //convert the guess to a double
 //   dbGuess = m_oStr->StringToDouble(m_szGuess);

    m_oStr->StringCopy(m_szEquation, _T("X-5"));
    dbGuess = 3;

    EqSolver(m_szEquation, _T("X"), dbGuess, dbAns, m_szAnswer);


    m_oStr->StringCopy(m_szEquation, _T("X^3-5"));
    EqSolver(m_szEquation, _T("X"), dbGuess, dbAns, m_szAnswer);

    m_oStr->StringCopy(m_szEquation, _T("(X)^2+(X)^2+5"));
    EqSolver(m_szEquation, _T("X"), dbGuess, dbAns, m_szAnswer);

    m_oStr->StringCopy(m_szEquation, _T("(sin(X)-0.5"));
    EqSolver(m_szEquation, _T("X"), dbGuess, dbAns, m_szAnswer);

    m_oStr->StringCopy(m_szEquation, _T("cos(X)+0.25"));
    EqSolver(m_szEquation, _T("X"), dbGuess, dbAns, m_szAnswer);


//Error:
    return hr;
}



/*********************************************************/
// 
// Pre:
// Post:
// Return Value:
// Intent:
/*********************************************************/
EnumError CEquationSolver::EqSolver(TCHAR* szEquation, TCHAR* szVar, double dbGuess, 
                             double &ldbAnswer, TCHAR* szAnswer)
{
    EnumError error = SUCCESSFUL;

    TCHAR szTempEq[STRING_MAX*2];
    TCHAR szTemp[STRING_LARGE];

    //long double doesn't help on mobile but no harm in leaving it in
    long double x1 = dbGuess;
    long double x2 = dbGuess;
    long double fx, fpx;

    int count(0);

    while(fabs(x2-x1) > SOLVER_DECIMAL_PRECISION || !count)
    {
        ++count;
        x1 = x2;

        if(count > MAX_SOLVE_LOOPS)
        {
            error = NO_ROOTS;
            goto Error;
        }

        //now calculate
        m_oStr->DoubleToString(szTemp, x1);
        m_oUtil->FindAndReplaceString(szEquation, szVar, szTemp, szTempEq);
        fx = m_oCalc->Calculate(szTempEq, szTemp);

     //   if(SUCCESSFUL != Compute(eq, var, x1, fx, NULL))
     //       return INVALID_EQ;

        // error checking
        // BUGBUG

        // calculate slope
        if(SUCCESSFUL != DyDx(szEquation, szVar, x1, fpx))
            return INVALID_EQ;

        // error checking
        if ((fpx == HUGE_VAL) || (fpx == -HUGE_VAL))
        {
            error = INFINITE_VALUE;
            goto Error;
        }

        if(count==1 && -1e-10 < fpx && fpx < 1e-10)
            return ZERO_SLOPE;

        x2 = x1 - (fx/fpx);
    }

    m_oStr->DoubleToString(m_szAnswer, x2);
    ldbAnswer = x2;

    return error;

Error:

    // lets see if bi-section method gets a better answer
    long double ldBiAns;
    error = EqSolverBisection(szEquation, szVar, dbGuess, ldBiAns);

    if(error == SUCCESSFUL)
    {
        m_oStr->DoubleToString(m_szAnswer, x2);
    }
    else
    {
        m_oStr->Empty(m_szAnswer);
        ldBiAns = 0.0;
    }


    return error;		
}

EnumError CEquationSolver::DyDx(TCHAR* szEquation, TCHAR* szVar, long double x, long double &ldbAns)
{
    TCHAR szTempEq[STRING_MAX*2];
    TCHAR szTemp[STRING_LARGE];

    long double x1, x2, y1, y2;
    x1 = x - DYDX_DELTA;
    x2 = x + DYDX_DELTA;

    m_oStr->DoubleToString(szTemp, x1);
    m_oUtil->FindAndReplaceString(szEquation, szVar, szTemp, szTempEq);
    y1 = m_oCalc->Calculate(szTempEq, szTemp);

    m_oStr->DoubleToString(szTemp, x2);
    m_oUtil->FindAndReplaceString(szEquation, szVar, szTemp, szTempEq);
    y2 = m_oCalc->Calculate(szTempEq, szTemp);

    ldbAns = ((y2-y1)/(x2-x1));

    return SUCCESSFUL;
}

EnumError CEquationSolver::EqSolverBisection(TCHAR* szEquation, TCHAR* szVar, double dbGuess, long double &ldAns)
{
    int percent = 10;
    long double range = fabs((double(percent)/100.0) * dbGuess);

    if(range < MIN_SOLVER_RANGE)
    {
        range = MIN_SOLVER_RANGE;
    }

    double min = dbGuess - range;
    double max = dbGuess + range;
    double a = min;
    double b = max;
    double a_old = 0;
    double b_old = 0;
    double p(0), p_old(1);
    double fp, fa, fb;
    int count(0);
    bool fGreaterThan = false;
    bool fLessThan = false;
    bool fBrokeOut = false;

    while(fabs(b-a) > SOLVER_DECIMAL_PRECISION)
    {
        ++count;

        p_old = p;
        p = (a+b)/2.0;

        if(p_old == p && count != 1)
        {
            fBrokeOut = true;
            break; // probably caught in a loop, break out
        }

        if(TRUE != Compute(szEquation, szVar, p, fp))
            return INVALID_EQ;

        if(a_old != a || count == 1)
            if(TRUE != Compute(szEquation, szVar, a, fa))
                return INVALID_EQ;

        a_old = a;

        if((fp == 0) && (fp != HUGE_VAL) && (fp != -HUGE_VAL))
            break; // we have exact answer
        else if((fa != HUGE_VAL) && (fa != -HUGE_VAL))
        {	
            if((fp*fa) < 0)
            {
                b = p;
                fLessThan = true;
            }
            else if((fp*fa) > 0)
            {
                a = p;
                fGreaterThan = true;
            }
        }
        else
        {
            if(b_old != b || count == 1)
                if(TRUE != Compute(szEquation, szVar, b, fb))
                    return INVALID_EQ;
            b_old = b;

            if((fb != HUGE_VAL) && (fb != -HUGE_VAL))
            {	
                if((fp*fb) < 0)
                {
                    a = p;
                    fGreaterThan = true;
                }
                else if((fp*fb) > 0)
                {
                    b = p;
                    fLessThan = true;
                }
            }
            else
            {
                // we need to narrow our range a bit
                count = 0;
                --percent;

                if(percent == 0)
                    return UNDEFINED_VALUE;

                a = min = dbGuess - fabs((double(percent)/100.0) * range);
                b = max = fabs((double(percent)/100.0) * range) + dbGuess;
                p = 0;
                fGreaterThan = fLessThan = false;
            }
        }
    }

    ldAns = p;

    // we just converged to one of the end points, we didn't really find a root
    if(fLessThan != fGreaterThan && ((fabs(a-max) < SOLVER_DECIMAL_PRECISION) || (fabs(b-min) < SOLVER_DECIMAL_PRECISION)))
        return NO_ROOTS;
    else if(fBrokeOut)
        return UNKNOWNERR;
    else if((fp == HUGE_VAL) || (fp == -HUGE_VAL))
        return NO_ROOTS;
    else
        return SUCCESSFUL;
}

BOOL CEquationSolver::Compute(TCHAR* szEquation, TCHAR* szVar, double& dbValue, double& dbAnswer)
{
    if(m_oStr->IsEmpty(szEquation) || m_oStr->IsEmpty(szVar))
        return FALSE;

    TCHAR szTemp[STRING_LARGE];
    TCHAR szTempEq[STRING_MAX*3];

    //now calculate
    m_oStr->DoubleToString(szTemp, dbValue);
    m_oUtil->FindAndReplaceString(szEquation, szVar, szTemp, szTempEq);
    dbAnswer = m_oCalc->Calculate(szEquation, szTemp);
    return TRUE;
}