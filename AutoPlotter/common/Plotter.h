#pragma once
#include "SerialPort.h"
#include <string>
using namespace std;


class CPlotter
{
public:
	CPlotter(void);
	~CPlotter(void);

	bool Open();

	void Close();

	bool BeginRun(const string &type,const string &id);

	bool WaitRunDone();

	void ResetError();

	string	GetLastError();
public:
	string	TransError(char error[4]);
private:
	string	m_lastError;
	SerialPort m_serialPort;

};
