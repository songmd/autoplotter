
// AutoPlotter1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AutoPlotter1.h"
#include "AutoPlotter1Dlg.h"
#include "afxdialogex.h"

#include "ConfrmDlg.h"
#include "OrderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoPlotter1Dlg 对话框




CAutoPlotter1Dlg::CAutoPlotter1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoPlotter1Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_productNo = 1;
	m_strOrder = _T("");
	m_strNote = _T("");
	m_nOrderTotalNum = 0;
	m_strOrderType = _T("");
	m_nOrderCompNum = 0;
}

void CAutoPlotter1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_logEdit);
	DDX_Control(pDX, IDC_BUTTON_RUN, m_btnRun);
	DDX_Text(pDX, IDC_EDIT_NO, m_productNo);
	DDV_MinMaxInt(pDX, m_productNo, 1, 200);

	DDX_Control(pDX, IDC_COMBO_ORDER, m_ctrlOrders);
	DDX_CBString(pDX, IDC_COMBO_ORDER, m_strOrder);
	DDX_Text(pDX, IDC_ORDER_NOTE, m_strNote);
	DDX_Text(pDX, IDC_ORDER_TOTAL, m_nOrderTotalNum);
	DDX_Text(pDX, IDC_EDIT_TYPE, m_strOrderType);
	DDX_Text(pDX, IDC_ORDER_COMPLETE, m_nOrderCompNum);


}

BEGIN_MESSAGE_MAP(CAutoPlotter1Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CAutoPlotter1Dlg::OnBnClickedButtonRun)
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_BUTTON_ADD, &CAutoPlotter1Dlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CAutoPlotter1Dlg::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CAutoPlotter1Dlg::OnBnClickedButtonDelete)
	ON_CBN_SELCHANGE(IDC_COMBO_ORDER, &CAutoPlotter1Dlg::OnSelchangeComboOrder)

END_MESSAGE_MAP()


// CAutoPlotter1Dlg 消息处理程序

BOOL CAutoPlotter1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	if( !m_plotter.Open() )
	{
		AfxMessageBox(_T("连接刻字机失败！"));
	}
	// TODO: 在此添加额外的初始化代码
	CTime ct = CTime::GetCurrentTime();
	m_today = CTime::GetCurrentTime();
	int time = AfxGetApp()->GetProfileInt(_T("setting"),_T("time"),0);
	int no = AfxGetApp()->GetProfileInt(_T("setting"),_T("no"),1);
	if (time == ct.GetYear()*100*100 +ct.GetMonth()*100+ct.GetDay())
	{
		m_productNo = no;
	}
	else
	{
		m_productNo = 1;
	}

	LoadOrders(m_vctOrders);

	CStringArray arOrderId;
	GetOrderId(m_vctOrders,arOrderId);
	
	
	for( int i=0; i<arOrderId.GetCount(); i++ )
	{
		m_ctrlOrders.AddString(arOrderId[i]);
	}

	CString strOrder = AfxGetApp()->GetProfileString(_T("setting"),_T("orderid"),_T(""));
	UpdateByOrder(strOrder);

	UpdateData(FALSE);

	LoadLogs(m_logs);

	UpdateLogView();

	SetTimer(101,300,NULL);
	SetTimer(102,100,NULL);//更新时间
	m_bNeedWait = false;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoPlotter1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAutoPlotter1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAutoPlotter1Dlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_plotter.Close();
	SaveSetting();
	SaveLogs(m_logs);
	CDialogEx::OnCancel();
}


void CAutoPlotter1Dlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CAutoPlotter1Dlg::OnBnClickedButtonRun()
{
	if( !UpdateData() )
		return;
	if( m_bNeedWait )
	{
		AfxMessageBox(_T("正在刻字，请等刻字完成再尝试扫码"));
		return;
	}
	size_t curSel = -1;
	for( size_t i=0; i<m_vctOrders.size(); i++ )
	{
		if( m_vctOrders[i].strId == m_strOrder )
		{
			curSel = i;
			break;
		}
	}
	if( curSel == -1 )
	{
		AfxMessageBox(_T("当前没有选中订单，或者订单无效！"));
		return ;
	}

	if( m_vctOrders[curSel].nCompNum == m_vctOrders[curSel].nTotalNum )
	{
		AfxMessageBox(_T("当前订单的产品已经全部完成！"));
		return ;
	}

	CConfrmDlg dlg;
	CTime ct = CTime::GetCurrentTime();
	dlg.m_strTime.Format(_T("%d年  %d月  %d日"),ct.GetYear(),ct.GetMonth(),ct.GetDay());
	dlg.m_strNO.Format(_T("当日第  %d  个"), m_productNo);

	dlg.m_strOrderNoDesc.Format(_T("当前订单的第  %d  个产品"),m_vctOrders[curSel].nCompNum+1);
	dlg.m_strOrderId = m_vctOrders[curSel].strId;
	dlg.m_strOrderType = m_vctOrders[curSel].strType;
	dlg.m_strNote = m_vctOrders[curSel].strNote;
	dlg.m_strProductId = CreateID(ct,m_productNo);
	int nCurrentNo = m_productNo;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		m_bNeedWait = true;
		m_btnRun.SetWindowText(_T("正在刻字......"));
		m_btnRun.EnableWindow(FALSE);
		int nOrderNo = m_vctOrders[curSel].nCompNum+1;
		if( m_plotter.BeginRun(W2M(m_vctOrders[curSel].strType),W2M(dlg.m_strProductId) ))
		{
			m_vctOrders[curSel].nCompNum += 1;
			SaveOrders(m_vctOrders);
			UpdateByOrder(m_strOrder);
			m_productNo += 1;
			if( m_productNo > 200 )
				m_productNo = 1;
			UpdateData(FALSE);
		}
		else
		{
			m_bNeedWait = false;
			m_btnRun.EnableWindow(TRUE);
			m_btnRun.SetWindowText(_T("开始刻字"));
			AfxMessageBox(_T("刻字失败"));
		}
		CreateLog(ct,nOrderNo,m_vctOrders[curSel].strId,nCurrentNo,m_vctOrders[curSel].strType,dlg.m_strProductId,M2W(m_plotter.GetLastError()));
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	
}


void CAutoPlotter1Dlg::SaveSetting(void)
{
	UpdateData();
		// TODO: 在此添加额外的初始化代码
	CTime ct = CTime::GetCurrentTime();

	int time = ct.GetYear()*100*100 +ct.GetMonth()*100+ct.GetDay();
	AfxGetApp()->WriteProfileInt(_T("setting"),_T("time"),time);
	AfxGetApp()->WriteProfileInt(_T("setting"),_T("no"),m_productNo);
	AfxGetApp()->WriteProfileString(_T("setting"),_T("orderid"),m_strOrder);

}

void	CAutoPlotter1Dlg::CreateLog(CTime ct,int ordno, CString orderid,int nNo,CString type,CString id,CString fail)
{
	CString	log;
	CString result;
	if( fail.IsEmpty() )
		result = _T("刻字成功！");
	else
		result.Format(_T("刻字失败：%s！"),fail);
	log.Format(_T("订单：%s   第%d个   当日第%d个工件   类型：%s   ID：%s    时间：%s   %s"),orderid,ordno,nNo,type,id,ct.Format(_T("%Y/%m/%d %H:%M:%S")),result);
	m_logs.Add(log);
	UpdateLogView();
}


void CAutoPlotter1Dlg::UpdateLogView(void)
{
	CString strText ;
	for (int i=0; i<m_logs.GetCount();i++)
	{
		strText += (m_logs[i]+_T("\r\n"));
	}
	m_logEdit.SetWindowText(strText);
	int cnt = m_logEdit.GetLineCount();
	m_logEdit.LineScroll(cnt, 0);
}


void CAutoPlotter1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if( nIDEvent == 101 )
	{
		if( m_bNeedWait )
		{
			bool bDone = m_plotter.WaitRunDone();
			if( bDone )
			{
				m_bNeedWait = false;
				m_btnRun.EnableWindow(TRUE);
				m_btnRun.SetWindowText(_T("开始刻字"));
				m_btnRun.SetFocus();

			}
		}
	}
	if( nIDEvent == 102 )
	{
		CTime ct = CTime::GetCurrentTime();
		if( ct.GetDay() != m_today.GetDay() )
		{
			m_today = ct;
			m_productNo = 1;
			UpdateData(FALSE);
			AfxMessageBox(_T("新的一天来临，产品序号重新从1开始计数。"));

		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


CString	CAutoPlotter1Dlg::CreateCode1(int no)
{
	TCHAR *codes = _T("ABCDEFGHJKLMNPQRSTUWXZ");
	while( no >= 220 )
	{
		no -= 220;
		no += 1;
	}	

	int nz = no / 10;
	int ny = no % 10;


	CString ret ;
	ret.Format(_T("%c%d"),codes[nz],ny);
	return ret;
}

CString	CAutoPlotter1Dlg::CreateCode2(int no)
{
	int n1 = no / 100;
	int n2 = (no / 10) % 10;
	int n3 = no % 10;
	CString ret ;
	ret.Format(_T("%02d"), n1 + n2 + n3);
	return ret;
}

CString CAutoPlotter1Dlg::CreateID(CTime ct,int nPNO)
{

	CTimeSpan span = ct - CTime(ct.GetYear(),1,1,0,0,0); 
	int days = span.GetDays()+1;//全年第多少天

	CString ret ;
	ret.Format(_T("Z%s%s%s-%s%s"),
		CreateCode1(ct.GetYear()-2010),
		CreateCode1(days),
		CreateCode1(nPNO),
		CreateCode2(days),
		CreateCode2(nPNO));
	return ret;
}


void CAutoPlotter1Dlg::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	COrderDlg dlg;
	dlg.m_bForEdit = false;

	GetOrderId(m_vctOrders,dlg.m_arOrderId);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		stOrder order;
		order.strId = dlg.m_strOrderId;
		order.strType = dlg.m_strOrderType;
		order.strNote = dlg.m_strOrderNote;
		order.nTotalNum = dlg.m_nOrderTotalNum;
		order.nCompNum = dlg.m_nOrderCompleteNum;
		m_vctOrders.push_back(order);
		m_ctrlOrders.AddString(order.strId);
		SaveOrders(m_vctOrders);
		UpdateByOrder(order.strId);
	}
	else if (nResponse == IDCANCEL)
	{
	}
}

void CAutoPlotter1Dlg::OnBnClickedButtonEdit()
{
	// TODO: Add your control notification handler code here
	size_t curSel = -1;
	for( size_t i=0; i<m_vctOrders.size(); i++ )
	{
		if( m_vctOrders[i].strId == m_strOrder )
		{
			curSel = i;
			break;
		}
	}
	if( curSel == -1 )
	{
		AfxMessageBox(_T("当前没有选中订单，或者订单无效！"));
		return ;
	}
	COrderDlg dlg;
	dlg.m_bForEdit = true;

	dlg.m_strOrderId = m_vctOrders[curSel].strId ;
	dlg.m_strOrderNote = m_vctOrders[curSel].strNote ;
	dlg.m_strOrderType = m_vctOrders[curSel].strType ;
	dlg.m_nOrderTotalNum = m_vctOrders[curSel].nTotalNum;
	dlg.m_nOrderCompleteNum = m_vctOrders[curSel].nCompNum;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		stOrder order;
		order.strId = dlg.m_strOrderId;
		order.strType = dlg.m_strOrderType;
		order.strNote = dlg.m_strOrderNote;
		order.nTotalNum = dlg.m_nOrderTotalNum;
		order.nCompNum = dlg.m_nOrderCompleteNum;
		m_vctOrders[curSel] = order;
		SaveOrders(m_vctOrders);
		UpdateByOrder(order.strId);
	}
	else if (nResponse == IDCANCEL)
	{
	}
}


void CAutoPlotter1Dlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	size_t curSel = -1;
	for( size_t i=0; i<m_vctOrders.size(); i++ )
	{
		if( m_vctOrders[i].strId == m_strOrder )
		{
			curSel = i;
			break;
		}
	}
	if( curSel == -1 )
	{
		AfxMessageBox(_T("当前没有选中订单，或者订单无效！"));
		return ;
	}
	m_ctrlOrders.DeleteString(m_ctrlOrders.FindString(0,m_vctOrders[curSel].strId));
	m_vctOrders.erase(m_vctOrders.begin()+curSel);
	SaveOrders(m_vctOrders);
	UpdateByOrder(m_strOrder);
}


void CAutoPlotter1Dlg::OnSelchangeComboOrder()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	UpdateByOrder(m_strOrder);

}

void CAutoPlotter1Dlg::UpdateByOrder(CString order)
{
	bool bFind = false;
	for(size_t i=0; i<m_vctOrders.size(); i++ )
	{
		if(order == m_vctOrders[i].strId)
		{
			m_strOrder = order;
			m_strOrderType = m_vctOrders[i].strType;
			m_strNote = m_vctOrders[i].strNote;
			m_nOrderTotalNum = m_vctOrders[i].nTotalNum;
			m_nOrderCompNum = m_vctOrders[i].nCompNum;
			bFind = true;
			UpdateData(FALSE);
			break;
		}
	}
	if( !bFind )
	{
		m_strOrder = _T("");
		m_strNote = _T("");
		m_nOrderTotalNum = 0;
		m_strOrderType = _T("");
		m_nOrderCompNum = 0;
		UpdateData(FALSE);
	}
}