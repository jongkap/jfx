// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       jerror.cpp
/// @brief      jerror Ŭ���� ����
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
/// @brief  ���� ��ȣ ����
/// @param  jerrno         [in] ���� ��ȣ
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
/// @brief  ���� ��ũ�� ����
/// @param  strerror       [in] ���� ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
void jerror::setstrerror(const char* strerror)
{
	m_strerror = strerror;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     void jerror::appendstrerror(const char* strerror)
/// @brief  ���� ��ũ�� �߰�
/// @param  strerror       [in] ���� ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
void jerror::appendstrerror(const char* strerror)
{
	m_strerror += strerror;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     const char* jerror::strerror() const
/// @brief  ���� ��Ʈ�� ���
/// @return ���� ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
const char* jerror::strerror() const
{
	return m_strerror.c_str();
}


__END_NAMESPACE_JFX
