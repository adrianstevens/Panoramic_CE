#pragma once

#include "calc.h"
#include "issstring.h"
#include "IssSciCalcUtil.h"

//messy but since its already done ....
enum EnumError
{
    SUCCESSFUL=0, 
    UNKNOWNERR, 
    NO_ROOTS, 
    INVALID_EQ, 
    UNDEFINED_VALUE,
    ZERO_SLOPE, 
    INFINITE_VALUE,
};



class CEquationSolver
{
public:
    CEquationSolver(void);
    ~CEquationSolver(void);

    HRESULT     SolveEquation();

    EnumError   EqSolver(TCHAR* szEquation, TCHAR* szVar, double dbGuess, 
                        double &ldbAnswer, TCHAR* szAnswer);
    EnumError   DyDx(TCHAR* szEquation, TCHAR* szVar, long double x, long double &ldbAns);
    EnumError   EqSolverBisection(TCHAR* szEquation, TCHAR* szVar, double dbGuess, long double &ldAns);

    BOOL        Compute(TCHAR* szEquation, TCHAR* szVar, double& dbValue, double& dbAnswer);
    

private:

private:
    SciCalc*            m_oCalc;
    CIssSciCalcUtil*    m_oUtil;
    CIssString*         m_oStr;

    TCHAR       m_szEquation[STRING_MAX*2];
    TCHAR       m_szGuess[STRING_LARGE];
    TCHAR       m_szAnswer[STRING_LARGE];
};
