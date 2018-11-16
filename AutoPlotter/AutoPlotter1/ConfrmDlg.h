#pragma once


// CConfrmDlg dialog

class CConfrmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfrmDlg)

public:
	CConfrmDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfrmDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strNO;
	CString m_strTime;

	CString m_strProductId;
	CString m_strNote;
	CString m_strOrderNoDesc;
	CString m_strOrderId;
	CString m_strOrderType;
	virtual BOOL OnInitDialog();
};
