
// AutoPlotter2Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoPlotter2.h"
#include "AutoPlotter2Dlg.h"
#include "afxdialogex.h"
#include "ScannerCommands.h"
#include "ConfirmDlg.h"
#include "QuickXmlParser.h"
#include "OrderDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoPlotter2Dlg �Ի���




CAutoPlotter2Dlg::CAutoPlotter2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoPlotter2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_strOrder = _T("");
	m_strNote = _T("");
	m_nOrderTotalNum = 0;
	m_strOrderType = _T("");
	m_nOrderCompNum = 0;
}

void CAutoPlotter2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_logEdit);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_editStatus);
	DDX_Control(pDX, IDC_COMBO_ORDER, m_ctrlOrders);
	DDX_CBString(pDX, IDC_COMBO_ORDER, m_strOrder);
	DDX_Text(pDX, IDC_ORDER_NOTE, m_strNote);
	DDX_Text(pDX, IDC_ORDER_TOTAL, m_nOrderTotalNum);
	DDX_Text(pDX, IDC_EDIT_TYPE, m_strOrderType);
	DDX_Text(pDX, IDC_ORDER_COMPLETE, m_nOrderCompNum);
}

BEGIN_MESSAGE_MAP(CAutoPlotter2Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CAutoPlotter2Dlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CAutoPlotter2Dlg::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CAutoPlotter2Dlg::OnBnClickedButtonDelete)
	ON_CBN_SELCHANGE(IDC_COMBO_ORDER, &CAutoPlotter2Dlg::OnSelchangeComboOrder)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CAutoPlotter2Dlg::OnBnClickedButtonTest)
END_MESSAGE_MAP()


// CAutoPlotter2Dlg ��Ϣ�������

BOOL CAutoPlotter2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	if( !m_plotter.Open() )
	{
		AfxMessageBox(_T("���ӿ��ֻ�ʧ�ܣ�"));
	}

	m_editStatus.SetWindowText(_T("�ȴ�ɨ��"));
	LoadLogs(m_logs);

	LoadOrders(m_vctOrders);

	CStringArray arOrderId;
	GetOrderId(m_vctOrders,arOrderId);
	
	
	for( int i=0; i<arOrderId.GetCount(); i++ )
	{
		m_ctrlOrders.AddString(arOrderId[i]);
	}

	CString strOrder = AfxGetApp()->GetProfileString(_T("setting"),_T("orderid"),_T(""));
	UpdateByOrder(strOrder);

	

	UpdateLogView();

	SetTimer(101,300,NULL);
	m_bNeedWait = false;
	m_bScanning = false;
	
	long status = 1;
	m_ScannerCommands = new CScannerCommands(this);
	SHORT ScannerTypesArray[TOTAL_SCANNER_TYPES] = {0, 0, 0, 0, 0, 0, 0, 0,0};

	ScannerTypesArray[0] = SCANNER_TYPES_ALL;

	status = m_ScannerCommands->cmdOpen(ScannerTypesArray, 1, &status);

	status = 1;
	m_ScannerCommands->cmdRegisterEvents(6, L"1,2,4,8,16,32", &status);
	//LOG(status, "REGISTER_FOR_ALL_EVENTS");
	//Initiate scanner discovery
	status = 1;
	BSTR outXml = 0;
	if ( m_ScannerCommands->cmdDiscover(&outXml, &status) == S_OK )
	{
	}

	status = 1;
	SC->cmdSwitchHostMode(_T("1"), _T("XUA-45001-1"), _T("FALSE"), _T("FALSE"), 0, &status);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAutoPlotter2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAutoPlotter2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAutoPlotter2Dlg::OnScanData(short reserved, BSTR scanData)
{
	if( m_bScanning )
		return ;
	m_bScanning = true;
	if( m_bNeedWait )
	{
		m_bScanning = false;
		AfxMessageBox(_T("���ڿ��֣���ȿ�������ٳ���ɨ��"));
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
		m_bScanning = false;
		AfxMessageBox(_T("��ǰû��ѡ�ж��������߶�����Ч��"));
		return ;
	}

	if( m_vctOrders[curSel].nCompNum == m_vctOrders[curSel].nTotalNum )
	{
		m_bScanning = false;
		AfxMessageBox(_T("��ǰ�����Ĳ�Ʒ�Ѿ�ȫ����ɣ�"));
		return ;
	}
	
	CQuickXmlParser x(scanData);
	CQuickXmlParser::TAGDATA tag[3] = {0};
	tag[0].Tag.Name = L"datatype";
	tag[1].Tag.Name = L"datalabel";
	tag[2].Tag.Name = L"rawdata";

	x.Configure(tag, 3);
	x.Parse();
	CString data = x.TranslateHex2Dec(tag[1].Value);
	
//	data = _T("ddds odiw uwuueieoei");
	int pos = data.ReverseFind(_T(' '));
	CString oldType;
	CString productId;
	if( pos > 0 )
	{
		oldType = data.Left(pos);
		productId = data.Right(data.GetLength()-pos-1);
	}
	else
	{
		oldType = data.Left(7);
		productId = data.Right(12);
	}

	CConfirmDlg dlg;
	CTime ct = CTime::GetCurrentTime();
	dlg.m_strProductId = productId;
	dlg.m_strTypeOld = oldType;
	dlg.m_strOrderNoDesc.Format(_T("��ǰ�����ĵ�  %d  ����Ʒ"),m_vctOrders[curSel].nCompNum+1);
	dlg.m_strOrderId = m_vctOrders[curSel].strId;
	dlg.m_strOrderType = m_vctOrders[curSel].strType;
	dlg.m_strNote = m_vctOrders[curSel].strNote;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		m_bNeedWait = true;
		m_editStatus.SetWindowText(_T("���ڿ���..."));
		int nOrderNo = m_vctOrders[curSel].nCompNum+1;
		if( m_plotter.BeginRun(W2M(m_vctOrders[curSel].strType),W2M(productId)))
		{
			m_vctOrders[curSel].nCompNum += 1;
			SaveOrders(m_vctOrders);
			UpdateByOrder(m_strOrder);
		}
		else
		{
			AfxMessageBox(_T("����ʧ��"));
			m_bNeedWait = false;
			m_editStatus.SetWindowText(_T("�ȴ�ɨ��"));
		}
		CreateLog(ct,nOrderNo,m_vctOrders[curSel].strId,m_vctOrders[curSel].strType,productId,M2W(m_plotter.GetLastError()));
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	m_bScanning = false;
}

void	CAutoPlotter2Dlg::CreateLog(CTime ct,int ordno, CString orderid,CString type,CString id,CString fail)
{
	CString	log;
	CString result;
	if( fail.IsEmpty() )
		result = _T("���ֳɹ���");
	else
		result.Format(_T("����ʧ�ܣ�%s��"),fail);
	log.Format(_T("������%s   ��%d��   ���ͣ�%s   ID��%s    ʱ�䣺%s   %s"),orderid,ordno,type,id,ct.Format(_T("%Y/%m/%d %H:%M:%S")),result);
	m_logs.Add(log);
	UpdateLogView();
}



void CAutoPlotter2Dlg::UpdateLogView(void)
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

void CAutoPlotter2Dlg::OnTimer(UINT_PTR nIDEvent)
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
				m_editStatus.SetWindowText(_T("�ȴ�ɨ��"));
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CAutoPlotter2Dlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	m_plotter.Close();
	AfxGetApp()->WriteProfileString(_T("setting"),_T("orderid"),m_strOrder);
	SaveLogs(m_logs);
	SaveOrders(m_vctOrders);
	CDialogEx::OnCancel();
}



void CAutoPlotter2Dlg::OnBnClickedButtonAdd()
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

void CAutoPlotter2Dlg::OnBnClickedButtonEdit()
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
		AfxMessageBox(_T("��ǰû��ѡ�ж��������߶�����Ч��"));
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


void CAutoPlotter2Dlg::OnBnClickedButtonDelete()
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
		AfxMessageBox(_T("��ǰû��ѡ�ж��������߶�����Ч��"));
		return ;
	}
	m_ctrlOrders.DeleteString(m_ctrlOrders.FindString(0,m_vctOrders[curSel].strId));
	m_vctOrders.erase(m_vctOrders.begin()+curSel);
	SaveOrders(m_vctOrders);
	UpdateByOrder(m_strOrder);
}


void CAutoPlotter2Dlg::OnSelchangeComboOrder()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	UpdateByOrder(m_strOrder);

}

void CAutoPlotter2Dlg::UpdateByOrder(CString order)
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

void CAutoPlotter2Dlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialogEx::OnOK();
}


void CAutoPlotter2Dlg::OnBnClickedButtonTest()
{
	// TODO: Add your control notification handler code here

	if( m_bScanning )
		return ;
	m_bScanning = true;
	if( m_bNeedWait )
	{
		m_bScanning = false;
		AfxMessageBox(_T("���ڿ��֣���ȿ�������ٳ���ɨ��"));
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
		m_bScanning = false;
		AfxMessageBox(_T("��ǰû��ѡ�ж��������߶�����Ч��"));
		return ;
	}

	if( m_vctOrders[curSel].nCompNum == m_vctOrders[curSel].nTotalNum )
	{
		m_bScanning = false;
		AfxMessageBox(_T("��ǰ�����Ĳ�Ʒ�Ѿ�ȫ����ɣ�"));
		return ;
	}
	/*
	CQuickXmlParser x(scanData);
	CQuickXmlParser::TAGDATA tag[3] = {0};
	tag[0].Tag.Name = L"datatype";
	tag[1].Tag.Name = L"datalabel";
	tag[2].Tag.Name = L"rawdata";

	x.Configure(tag, 3);
	x.Parse();
	CString data = x.TranslateHex2Dec(tag[1].Value);
	*/
	CString data = _T("ddds odiw uwuueieoei");
	int pos = data.ReverseFind(_T(' '));
	CString oldType;
	CString productId;
	if( pos > 0 )
	{
		oldType = data.Left(pos+1);
		productId = data.Right(data.GetLength()-pos-1);
	}
	else
	{
		oldType = data.Left(7);
		data.Right(12);
	}

	CConfirmDlg dlg;
	CTime ct = CTime::GetCurrentTime();
	dlg.m_strProductId = productId;
	dlg.m_strTypeOld = oldType;
	dlg.m_strOrderNoDesc.Format(_T("��ǰ�����ĵ�  %d  ����Ʒ"),m_vctOrders[curSel].nCompNum+1);
	dlg.m_strOrderId = m_vctOrders[curSel].strId;
	dlg.m_strOrderType = m_vctOrders[curSel].strType;
	dlg.m_strNote = m_vctOrders[curSel].strNote;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		m_bNeedWait = true;
		m_editStatus.SetWindowText(_T("���ڿ���..."));
		int nOrderNo = m_vctOrders[curSel].nCompNum+1;
		if( m_plotter.BeginRun(W2M(m_vctOrders[curSel].strType),W2M(productId)))
		{
			m_vctOrders[curSel].nCompNum += 1;
			SaveOrders(m_vctOrders);
			UpdateByOrder(m_strOrder);
		}
		else
		{
			AfxMessageBox(_T("����ʧ��"));
			m_bNeedWait = false;
			m_editStatus.SetWindowText(_T("�ȴ�ɨ��"));
		}
		CreateLog(ct,nOrderNo,m_vctOrders[curSel].strId,m_vctOrders[curSel].strType,productId,M2W(m_plotter.GetLastError()));
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	m_bScanning = false;
}
