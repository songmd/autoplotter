#include "StdAfx.h"
#include "common.h"
#include <fstream>


void LoadTypes(CStringArray & arTypes)
{
	CString path; 
    GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos)+_T("\\types.txt");
	ifstream fin(W2M(path));  
    string s;  
    while( getline(fin,s))
    {   
		CString strType = M2W(s);
		strType.Trim();
		if( !strType.IsEmpty() )
			arTypes.Add(strType);

    }

}

void SaveLogs(const CStringArray & logs)
{
	CString path; 
    GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos);   
	CTime ct = CTime::GetCurrentTime();

	CString logFileName;
	logFileName.Format(_T("%s\\logs\\%d-%d-%d.et"),path,ct.GetYear(),ct.GetMonth(),ct.GetDay());

//	file.Open(m_logFileName,CFile::modeWrite|CFile::modeCreate);
	ofstream log(W2M(logFileName) ) ;

	for (int i=0; i<logs.GetCount();i++)
	{
		log << W2M(logs[i]) << endl;
	}
}

void LoadLogs(CStringArray & logs)
{
	CString path; 
    GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos);   
	CTime ct = CTime::GetCurrentTime();
	CStdioFile file;
	CString logFileName;
	logFileName.Format(_T("%s\\logs\\%d-%d-%d.et"),path,ct.GetYear(),ct.GetMonth(),ct.GetDay());


	ifstream fin(W2M(logFileName));  
    string s;  
    while( getline(fin,s))
    {   
		logs.Add(M2W(s));
    }

}

void	LoadOrders(vector<stOrder> & vctOrders)
{
	CString path; 
    GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos)+_T("\\orders.txt");
	ifstream fin(W2M(path));  
    string s;  
    while( getline(fin,s))
    {   
		CString strOrder = M2W(s);
		CStringArray ar;
		SplitString(strOrder,_T('	'),ar);
		if( ar.GetCount() == 5 )
		{
			stOrder order;
			order.strId = ar[0];
			order.strType = ar[1];
			order.strNote = ar[2];
			order.nTotalNum =_ttoi(ar[3]);
			order.nCompNum = _ttoi(ar[4]);
			vctOrders.push_back(order);
		}
    }

}
void	SaveOrders(const vector<stOrder> & vctOrders)
{
	CString path; 
    GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
    path.ReleaseBuffer(); 
    int pos = path.ReverseFind('\\'); 
    path = path.Left(pos)+_T("\\orders.txt");
	ofstream fout(W2M(path));  
	for (int i=0; i<vctOrders.size();i++)
	{
		CString strOrder;
		strOrder += vctOrders[i].strId;
		strOrder += _T('	');
		strOrder += vctOrders[i].strType;
		strOrder += _T('	');
		strOrder += vctOrders[i].strNote;
		strOrder += _T('	');
		CString strTotalNum;
		strTotalNum.Format(_T("%d"),vctOrders[i].nTotalNum);
		strOrder += strTotalNum;
		strOrder += _T('	');
		CString strCompNum;
		strCompNum.Format(_T("%d"),vctOrders[i].nCompNum);
		strOrder += strCompNum;
		fout << W2M(strOrder) << endl;
	}
}
void	GetOrderId(const vector<stOrder> & vctOrders,CStringArray &arOrderId)
{
	for (int i=0; i<vctOrders.size();i++)
	{
		arOrderId.Add(vctOrders[i].strId);
	}
}

string	W2M(const CString &src)
{
	UINT _acp = ATL::_AtlGetConversionACP();
	int size = WideCharToMultiByte(_acp,0,(LPCTSTR)src,-1,0,0,0,0)+1;
	char *p = new char[size];
	memset(p,0,size);
	WideCharToMultiByte(_acp,0,(LPCTSTR)src,-1,p,size-1,0,0);
	string ret(p);
	delete p;
	return ret;
}
CString	M2W(const string &src)
{
	wchar_t * lpw = NULL;
	UINT _acp = ATL::_AtlGetConversionACP();
	int dsize = MultiByteToWideChar(_acp, 0, (LPSTR)src.c_str(), src.size(),0, 0)+1;
	lpw = new wchar_t[dsize];
	wmemset(lpw,0,dsize);
	int nRet = MultiByteToWideChar(_acp, 0, (LPSTR)src.c_str(), src.size(),  lpw, dsize-1);
	CString ret(lpw);
	delete [] lpw;
	return ret;
}
int SplitString(const CString str, TCHAR split, CStringArray &strArray)
{
	strArray.RemoveAll();
	CString strTemp = str;
	int iIndex = 0;
	while (1)
	{
		iIndex = strTemp.Find(split);
		if(iIndex >= 0)
		{
			strArray.Add(strTemp.Left(iIndex));
			strTemp = strTemp.Right(strTemp.GetLength()-iIndex-1);
		}
		else
		{
			break;
		}
	}
	strArray.Add(strTemp);
	return strArray.GetSize();
}