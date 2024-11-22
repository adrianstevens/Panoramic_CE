#pragma once
#include "dlgwsheet.h"
#include "IssLeaseCalc.h"

class CDlgWSheetLease :  public CDlgWSheet
{
public:
    CDlgWSheetLease(void);
    ~CDlgWSheetLease(void);

private:

private:
    CIssLeaseCalc       m_oLease;
};
