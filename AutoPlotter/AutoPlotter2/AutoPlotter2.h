
// AutoPlotter2.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutoPlotter2App:
// �йش����ʵ�֣������ AutoPlotter2.cpp
//

class CAutoPlotter2App : public CWinApp
{
public:
	CAutoPlotter2App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutoPlotter2App theApp;