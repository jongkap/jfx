// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       jmutex.cpp
/// @brief      jmutex 클래스 구현
/// @author     Jongkap Jeong <kbroad@mail.com, kbroad.blog.com>
/// @date       2007/04/30 - First Release
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#include "jfx.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#if defined(_WIN32)
  #define JFXMUTEX_TYPE			CRITICAL_SECTION
#else 
  #define JFXMUTEX_TYPE			pthread_mutex_t
#endif


__BEGIN_NAMESPACE_JFX

jmutex::jmutex()
{
	m_mutex = new JFXMUTEX_TYPE;

#if defined(_WIN32)
	InitializeCriticalSection((JFXMUTEX_TYPE*)m_mutex);
	//m_mutex = CreateMutex(NULL, FALSE, "xdbapi");
#else 
	//pthread_mutex_init(&m_mutex, NULL);
	//m_mutex = new JFXMUTEX_TYPE;
	pthread_mutex_init((JFXMUTEX_TYPE*)m_mutex, NULL);
#endif
}

jmutex::~jmutex()
{
#if defined(_WIN32)
	DeleteCriticalSection((JFXMUTEX_TYPE*)m_mutex);
	//CloseHandle(m_mutex);
	//delete (JFXMUTEX_TYPE*)m_mutex;
#else 
	 //pthread_mutex_destroy(&m_mutex);
	pthread_mutex_destroy((JFXMUTEX_TYPE*)m_mutex);	
#endif

	delete (JFXMUTEX_TYPE*)m_mutex;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     inline int jmutex::lock()
/// @brief  락 설정
/// @return 성공 여부
///////////////////////////////////////////////////////////////////////////////
int jmutex::lock()
{
#if defined(_WIN32)
	EnterCriticalSection((JFXMUTEX_TYPE*)m_mutex); 
	//return (WaitForSingleObject(m_mutex, INFINITE)==WAIT_FAILED?1:0);
#else 
	//return pthread_mutex_lock(&m_mutex);
	return pthread_mutex_lock((JFXMUTEX_TYPE*)m_mutex);
#endif

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     inline int jmutex::unlock()
/// @brief  락 해제
/// @return 성공 여부
///////////////////////////////////////////////////////////////////////////////
int jmutex::unlock()
{
#if defined(_WIN32)
	LeaveCriticalSection((JFXMUTEX_TYPE*)m_mutex);
	//return (ReleaseMutex(m_mutex)==0);
#else 
	//return pthread_mutex_unlock(&m_mutex);
	return pthread_mutex_unlock((JFXMUTEX_TYPE*)m_mutex);
#endif

	return 0;
}

__END_NAMESPACE_JFX
