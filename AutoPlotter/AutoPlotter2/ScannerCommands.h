/*******************************************************************************************
*
* ?016 Symbol Technologies LLC. All rights reserved.
*
********************************************************************************************/
#pragma once

#include "_CoreScanner_i.c"
#include "_CoreScanner.h"
#include "scannerdef.h"

#include <string>
#include "EventSink.h"

using namespace std;

#include "AutoPlotter2Dlg.h"

class CScannerCommands : public CCmdTarget
{
	DECLARE_DYNAMIC(CScannerCommands)

public:
	CScannerCommands();
	CScannerCommands(CAutoPlotter2Dlg * ParaAppDialog);
	virtual ~CScannerCommands();

protected:
	DECLARE_MESSAGE_MAP()
	

public:
	long cmdCreateInstance(void);
	void cmdDestroyInstance();
	long cmdOpen(SHORT [],SHORT,long *Status);
	long cmdClose(long *Status);
	long cmdDiscover(BSTR *,long *Status);
	long cmdGetAll(wstring,BSTR *,int,long *Status);
	long cmdGetNext(wstring,wstring,BSTR * outXml,int,long *Status);
	long cmdGet(wstring,wstring,BSTR * outXml,int,long *Status);
	long cmdExecute(DWORD nCmdID,wstring,BSTR * outXml,int,long *Status);
	long cmdBeep(wstring,wstring,int,long *Status);
	long cmdRegisterEvents(int,wstring,long *Status);
	long cmdUnRegisterEvents(int,wstring,long *Status);
	long cmdSet(wstring,wstring,int,long *Status);
	long cmdStore(wstring,wstring,int,long *Status);
	long cmdReboot(wstring,int,long *Status);
	long cmdDisconnect(wstring,int,long *Status);
	long cmdClaim(wstring,int,long *Status);
	long cmdRelease(wstring,int,long *Status);
	long cmdEnable(wstring,int,long *Status);
	long cmdDisable(wstring,int,long *Status);
	long cmdAimOff(wstring,int,long *Status);
	long cmdAimOn(wstring,int,long *Status);
	long cmdLED(wstring,wstring,int,long *Status,SCANNER* p);
	long cmdLEDOff(wstring,wstring,int,long *Status,SCANNER* p);
	long cmdSetParametersEx(wstring,wstring,wstring,int,long *Status);
	long cmdViewFinderParameter(wstring,wstring,wstring,int,long *Status);
	long cmdGetParameters(wstring,wstring,int,long *Status);
	long cmdSetParaDefault(wstring,int,long *Status);
	long cmdSetParaPersistanceEx(wstring,wstring,wstring,int,long *Status);
	long cmdGetScanCapa(wstring,BSTR * outXml,int,long *Status);
	long cmdGetScanSDKV(BSTR * outXml,int,long *Status);
	long cmdGetDevTopology(BSTR * outXml,int,long *Status);
	long cmdUpdateFW(wstring,wstring,int,long *Status, bool IsBulk);
	long cmdUpdateFWAbort(wstring,int,long *Status);
	long cmdStartNewFW(wstring,int,long *Status);
	long cmdSwitchHostMode(wstring, wstring, wstring, wstring, int, long *Status);
	long cmdUpdateAttribMeta(wstring,wstring,int,long *Status);
	long cmdFlushMacroPdf(wstring,int,long *Status);
	long cmdAbortMacroPdf(wstring,int,long *Status);
	long cmdPullTrigger(wstring,int,long *Status);
	long cmdReleaseTrigger(wstring,int,long *Status);
	long cmdCaptureImage(wstring,int,long *Status);
	long cmdSetBarcodeMode(wstring ScannerID, int Async, long *Status);
	long cmdCaptureVideo(wstring,int,long *Status);
	long cmdAbortImageXfer(wstring,int,long *Status);
	long cmdSetSerialInterface(wstring ScannerID, wstring BaudSettings, int Async, long *Status);
    long cmdGetBluetoothPairingBarcode(wstring ScannerID,int Async,long *Status, int protocol, int defailtOption, int size, wstring FilePath);

	long cmdEnableKeyboardEmulator(bool bEnable, int Async, long *Status); //VRQW74
	long cmdSetKeyboardEmulatorLocale(int Lang, int Async, long *Status); //VRQW74
	long cmdGetKeyboardEmulatorConfig(bool& bEnable, int& Lang, int Async, long *Status); //VRQW74
	long cmdSetDADFSource(wstring Source, int Async, long *Status); //VRQW74
	long cmdResetDADFSource(int Async, long *Status); //VRQW74

	long cmdReadWeight(wstring,BSTR * outXml,int,long *Status);
	long cmdZeroScale(wstring,int,long *Status);
	long cmdRestScale(wstring,int,long *Status);


	static CScannerCommands* GetScannerCommand(){ return m_pThis;}

private:
	HRESULT Execute(LONG opcode, BSTR* inXML, BSTR* outXML, int Async, LONG* status);

private:

	DWORD m_dwCookie;
	CEventSink * ScannerEventSink;
	ICoreScanner *ScannerInterface;
	LPUNKNOWN ScannerEventSinkUnknown;
	CAutoPlotter2Dlg * m_AppDialog;

	static CScannerCommands *m_pThis;

public:
	// Update Decode Tone
	long cmdUpdateDecodeTone(wstring ScannerID, wstring FilePath, long* Status);
	// Erase Decode Tone
	long cmdEraseDecodeTone(wstring ScannerID, long* Status);
};

#define SC (CScannerCommands::GetScannerCommand())
#define CHECK_CMD(x) {if (CScannerCommands::GetScannerCommand() == 0) {AfxMessageBox(L"Command Unavailable", MB_OK|MB_ICONINFORMATION );(return x;}}
#define CHECK_CMD0 {if (CScannerCommands::GetScannerCommand() == 0) {AfxMessageBox(L"Command Unavailable", MB_OK|MB_ICONINFORMATION );return;}}

