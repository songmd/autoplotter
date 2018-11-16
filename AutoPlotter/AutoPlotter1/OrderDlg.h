#pragma once


// COrderDlg dialog

class COrderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COrderDlg)

public:
	COrderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COrderDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ORDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strOrderId;
	CString m_strOrderNote;
	int m_nOrderTotalNum;
	int m_nOrderCompleteNum;
	CComboBox m_ctrlType;
	CString m_strOrderType;
	CEdit m_ctrlCompleteEdit;
	CStringArray	m_arOrderId;
	bool	m_bForEdit;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CEdit m_ctrlIdEdit;
private:
	void	SaveSetting();
};
