// OrderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPlotter2.h"
#include "OrderDlg.h"
#include "afxdialogex.h"
#include "../common/common.h"


// COrderDlg dialog

IMPLEMENT_DYNAMIC(COrderDlg, CDialogEx)

COrderDlg::COrderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COrderDlg::IDD, pParent)
{

	m_strOrderId = _T("");
	m_strOrderNote = _T("");
	m_nOrderTotalNum = 1;
	m_nOrderCompleteNum = 0;
	m_strOrderType = _T("");
	m_bForEdit = false;
}

COrderDlg::~COrderDlg()
{
}

void COrderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ORDER_ID, m_strOrderId);
	DDX_Text(pDX, IDC_ORDER_NOTE, m_strOrderNote);
	DDX_Text(pDX, IDC_ORDER_TOTAL, m_nOrderTotalNum);
	DDV_MinMaxInt(pDX, m_nOrderTotalNum, 1, 1000000);
	DDX_Text(pDX, IDC_ORDER_COMPLETE, m_nOrderCompleteNum);
	DDV_MinMaxInt(pDX, m_nOrderCompleteNum, 0, 100000);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ctrlType);
	DDX_CBString(pDX, IDC_COMBO_TYPE, m_strOrderType);
	DDX_Control(pDX, IDC_ORDER_COMPLETE, m_ctrlCompleteEdit);
	DDX_Control(pDX, IDC_ORDER_ID, m_ctrlIdEdit);
}


BEGIN_MESSAGE_MAP(COrderDlg, CDialogEx)
END_MESSAGE_MAP()


// COrderDlg message handlers


BOOL COrderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	if( !m_bForEdit )
		m_ctrlCompleteEdit.EnableWindow(FALSE);
	if( m_bForEdit )
		m_ctrlIdEdit.EnableWindow(FALSE);

	if( m_bForEdit )
	{
		SetWindowText(_T("修改订单"));
	}
	else
	{	
		SetWindowText(_T("添加订单"));
	}

	if( !m_bForEdit )
	{
		m_strOrderType = AfxGetApp()->GetProfileString(_T("setting"),_T("type"),_T(""));
	}
	
	CStringArray arTypes;
	LoadTypes(arTypes);

	for( int i=0;i<arTypes.GetCount(); i++)
	{
		CString t = arTypes.GetAt(i);
		t = t.Trim();
		if( !t.IsEmpty() )
		{
			m_ctrlType.AddString(t);
		}
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	COrderDlg::SaveSetting()
{
	AfxGetApp()->WriteProfileString(_T("setting"),_T("type"),m_strOrderType);

}

void COrderDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	if( !m_bForEdit )
	{
		for( int i=0; i<m_arOrderId.GetCount(); i++ )
		{
			if( m_strOrderId.CompareNoCase(m_arOrderId[i]) == 0 )
			{
				AfxMessageBox(_T("订单的编号已存在!"));
				return ;
			}
		}
	}
	if( m_strOrderId.IsEmpty() )
	{
		AfxMessageBox(_T("订单的编号不能为空!"));
		return ;
	}
	if( m_strOrderType.IsEmpty() )
	{
		AfxMessageBox(_T("订单的产品型号不能为空!"));
		return ;
	}
	if( m_nOrderTotalNum < m_nOrderCompleteNum )
	{
		AfxMessageBox(_T("订单的产品已完成数量不能大于产品总数!"));
		return ;
	}

	if( m_ctrlType.FindString(0,m_strOrderType) < 0 )
	{
		m_ctrlType.AddString(m_strOrderType);
	}
	SaveSetting();
	
	CDialogEx::OnOK();
}

