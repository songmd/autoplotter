#include "StdAfx.h"
#include "Plotter.h"


#define		CR			0x0D
#define		LF			0x0A
#define		NAK			0x15
#define		ACK			0x06
#define		ENQ			0x05
#define		EOT			0x04



CPlotter::CPlotter(void)
{
}


CPlotter::~CPlotter(void)
{
}

bool CPlotter::Open()
{
	return m_serialPort.Open("com1",9600,8,0,1,true);
}

void CPlotter::Close()
{
	m_serialPort.Close();
}


bool CPlotter::WaitRunDone()
{
//	Sleep(5000);
//	return true;
	char	buffer[16] = {0};
	m_serialPort.Read(buffer,1,20);

	if( buffer[0] == ENQ )
		return true;
	return false;
}

bool CPlotter::BeginRun(const string &type,const string &id)
{
	m_lastError = "";
//	Sleep(500);
//	return true;
	string load = "LOADFILE AUTO";
	load += LF;
	m_serialPort.Clear();
	m_serialPort.Write((void*)load.c_str(),load.size());
	char	buffer[256] = {0};
	m_serialPort.Read(buffer,11,200);
	if( strcmp(buffer,"LOADFILE OK") != 0 )
	{
		m_lastError = "装载文件失败";
		return false;
	}

	string setvartype = "SETVAR TYPE ";
	setvartype += type;
	setvartype += LF;
	m_serialPort.Clear();
	m_serialPort.Write((void*)setvartype.c_str(),setvartype.size());
	memset(buffer,0,256);
	m_serialPort.Read(buffer,9,200);

	if( strcmp(buffer,"SETVAR OK") != 0 )
	{
		m_lastError = "设置变量TYPE失败";
		return false;
	}

	m_serialPort.Clear();
	string setvarid = "SETVAR ID ";
	setvarid += id;
	setvarid += LF;
	m_serialPort.Clear();
	m_serialPort.Write((void*)setvarid.c_str(),setvarid.size());
	memset(buffer,0,256);
	m_serialPort.Read(buffer,9,200);

	if( strcmp(buffer,"SETVAR OK") != 0 )
	{
		m_lastError = "设置变量ID失败";
		return false;
	}
	m_serialPort.Clear();
	string run = "RUN";
	run += LF;
	m_serialPort.Write((void*)run.c_str(),run.size());
	
	memset(buffer,0,256);
	m_serialPort.Read(buffer,6,200);

	if( strcmp(buffer,"RUN OK") != 0 )
	{
		m_lastError = TransError(buffer);
		return false;
	}
	return true;
}


void CPlotter::ResetError()
{
	string reset = "RESETERROR ";
	reset += LF;
	m_serialPort.Clear();
	m_serialPort.Write((void*)reset.c_str(),reset.size());
	char	buffer[256] = {0};
	m_serialPort.Read(buffer,13,200);
	if( strcmp(buffer,"RESETERROR OK") != 0 )
	{
		return;
	}
}
string	CPlotter::TransError(char error[4])
{
	string ret="未知错误";
	if( error[0] == NAK )
	{
		char buf[64] = {0};
		sprintf(buf,"错误码 %02x %02x %02x",error[1],error[2],error[3]);
		ret = buf;
	}
	return ret;
}
string	CPlotter::GetLastError()
{
	return m_lastError;
}
