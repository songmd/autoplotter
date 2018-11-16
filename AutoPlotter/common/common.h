#pragma once

#include <string>
#include <vector>
using namespace std;

struct stOrder{
	CString		strId;
	CString		strType;
	CString		strNote;
	int			nTotalNum;
	int			nCompNum;
};

string	W2M(const CString &src);
CString	M2W(const string &src);
void LoadTypes(CStringArray & arTypes);
void	SaveLogs(const CStringArray & logs);
void	LoadLogs(CStringArray & logs);
void	LoadOrders(vector<stOrder> & vctOrders);
void	SaveOrders(const vector<stOrder> & vctOrders);
void	GetOrderId(const vector<stOrder> & vctOrders,CStringArray &arOrderId);
int		SplitString(const CString str, TCHAR split, CStringArray &strArray);

