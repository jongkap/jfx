// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       jerror.cpp
/// @brief      jerror 클래스 구현
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
  #ifndef strerror_r
    #define strerror_r(errno,buf,len) strerror_s(buf,len,errno)
  #endif 
#endif


__BEGIN_NAMESPACE_JFX


jerror::jerror()
{
	m_errno = 0;
	m_strerror = "errno[0]: no_error.";
}

jerror::~jerror()
{
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     void jerror::seterrno(int jerrno)
/// @brief  에러 번호 설정
/// @param  jerrno         [in] 에러 번호
///////////////////////////////////////////////////////////////////////////////
void jerror::seterrno(int jerrno)
{
	stringstream ss;
	char msg_buff[1024+1] = {0, };

	m_errno = jerrno;
	ss << m_errno;

	strerror_r(m_errno, msg_buff, 1024);

	m_strerror = "errno[";
	m_strerror += ss.str();
	m_strerror += "]: ";
	m_strerror += msg_buff;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     void jerror::setstrerror(const char* strerror)
/// @brief  에러 스크링 설정
/// @param  strerror       [in] 에러 스트링
///////////////////////////////////////////////////////////////////////////////
void jerror::setstrerror(const char* strerror)
{
	m_strerror = strerror;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     void jerror::appendstrerror(const char* strerror)
/// @brief  에러 스크링 추가
/// @param  strerror       [in] 에러 스트링
///////////////////////////////////////////////////////////////////////////////
void jerror::appendstrerror(const char* strerror)
{
	m_strerror += strerror;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     const char* jerror::strerror() const
/// @brief  에러 스트링 얻기
/// @return 에러 스트링
///////////////////////////////////////////////////////////////////////////////
const char* jerror::strerror() const
{
	return m_strerror.c_str();
}


__END_NAMESPACE_JFX
