#include "StdAfx.h"
#include "synctool.h"


/////////////////////////////////////////////////////////////////////////////////
//							UCriticalSection
///////////////////////////////////////////////////////////////////////////////////////////

UCriticalSection::UCriticalSection() 
{ 
	::InitializeCriticalSection(&m_sect);
}
UCriticalSection::~UCriticalSection() 
{ 
	::DeleteCriticalSection(&m_sect); 
}
void	UCriticalSection::Lock() 
{
	::EnterCriticalSection(&m_sect); 
}
void	UCriticalSection::Unlock() 
{
	::LeaveCriticalSection(&m_sect);
}


////////////////////////////////////////////////////////////////////////////////////////////
//							USecurityAttributes
///////////////////////////////////////////////////////////////////////////////////////////

USecurityAttributes::USecurityAttributes() {

	m_bInit = false ;
	// now, initialize the attributes ----------------------------------------------
	SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes.bInheritHandle = TRUE;
	// adjust pointer to fit in 8 byte boundaries
	SecurityAttributes.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)((DWORD)(&(SecurityDescriptorBase[7])) & 0xfffffff8);

	PSECURITY_DESCRIPTOR pSD = SecurityAttributes.lpSecurityDescriptor; 

	if( InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) )
	{
		if( SetSecurityDescriptorDacl(
			pSD, 
			TRUE,           // specifying a disc. ACL
			(PACL) NULL,    // means ALL ACCESS
			FALSE))	        // not a default disc. ACL
		{
			m_bInit = true ;
		} 
	} 
}
USecurityAttributes::~USecurityAttributes() {;}

USecurityAttributes::operator LPSECURITY_ATTRIBUTES () 
{
	if( m_bInit )
		return &SecurityAttributes;
	else
		return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////
//							UMutex
///////////////////////////////////////////////////////////////////////////////////////////

UMutex::UMutex( const char* name )
{
	USecurityAttributes	secAttr ;
	m_mutex = ::CreateMutexA(secAttr, FALSE, name );
}
UMutex::~UMutex()
{
	::CloseHandle(m_mutex); 
}
void UMutex::Lock() 
{
	WaitForSingleObject(m_mutex, INFINITE);
}
void UMutex::Unlock() 
{
	::ReleaseMutex(m_mutex); 
}


////////////////////////////////////////////////////////////////////////////////////////////
//							USingleLock
///////////////////////////////////////////////////////////////////////////////////////////

USingleLock::USingleLock( USyncTool	 *pSect) 
{
	m_pSect = pSect;
	m_pSect->Lock() ;
}

USingleLock::~USingleLock() 
{
	m_pSect->Unlock() ;
}


////////////////////////////////////////////////////////////////////////////////////////////
//							UNotifier
///////////////////////////////////////////////////////////////////////////////////////////

UNotifier::UNotifier(const char*name) 
{
	USecurityAttributes	secAttr;
	m_hNotifier = ::CreateEventA( secAttr ,FALSE,FALSE,name) ;
}
UNotifier::~UNotifier() 
{
	::CloseHandle( m_hNotifier ) ;
}
void	UNotifier::Notify() 
{
	::SetEvent( m_hNotifier ) ;
}
bool	UNotifier::WaitNotify( unsigned	milliseconds ) 
{
	return	WaitForSingleObject( m_hNotifier , milliseconds ) == WAIT_OBJECT_0 ; 
}

