
// AutoPlotter1.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutoPlotter1App:
// �йش����ʵ�֣������ AutoPlotter1.cpp
//

class CAutoPlotter1App : public CWinApp
{
public:
	CAutoPlotter1App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutoPlotter1App theApp;