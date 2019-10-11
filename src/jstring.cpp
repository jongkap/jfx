// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       jstring.cpp
/// @brief      jstring 클래스 구현
/// @author     Jongkap Jeong <jongkap@mail.com>
/// @date       2008/11/13 - 처음작성
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdarg.h>
#include <algorithm>

#include "jfx.h"

__BEGIN_NAMESPACE_JFX

jstring::jstring()
{
}

jstring::jstring(const jstring& cstr)
{
	m_str.assign(cstr.self());
}

jstring::jstring(const jstring& cstr, size_t pos, size_t len)
{
	m_str.assign(cstr.self(), pos, len);
}

jstring::jstring(const string& str)
{
	m_str.assign(str);
}

jstring::jstring(const string& str, size_t pos, size_t len)
{
	m_str.assign(str, pos, len);
}

jstring::jstring(const char* s)
{
	m_str.assign(s);
}

jstring::jstring(const char* s, size_t n)
{
	m_str.assign(s, n);
}

jstring::jstring(size_t n, char c)
{
	m_str.assign(n, c);
}

jstring::~jstring()
{
	m_str.clear();
}


//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::format(const char* fmt, ...)
/// @brief  포맷 스트링
/// @param  fmt         [in] 포맷
/// @param  ...         [in] 인수리스트
/// @return 포맷 스트링
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::format(const char* fmt, ...)
{
    size_t size = (strlen(fmt)) * 2 + 50;   // Use a rubric appropriate for your code
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        m_str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)m_str.data(), size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < (int)size) {  // Everything worked
            m_str.resize(n);
            return *this;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    
	return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::toupper()
/// @brief  대문자 변환
/// @return 대문자 스트링
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::toupper()
{
	std::transform(m_str.begin(), m_str.end(), m_str.begin(), ::toupper);

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::tolower()
/// @brief  소문자 변환
/// @return 소문자 스트링
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::tolower()
{
	std::transform(m_str.begin(), m_str.end(), m_str.begin(), ::tolower);

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::trim(const char* delim)
/// @brief  공백 제거
/// @param  delim   [in] 구분자
/// @return 공백 제거된 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::trim(const char* delim)
{
    // 글자 양쪽(왼쪽/오른쪽) 지정된 문자 제거
	rtrim(delim);
	ltrim(delim);

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn string& jstring::rtrim(const char* delim)
/// @brief  오른쪽 공백 제거
/// @param  delim   [in] 구분자
/// @return 오른쪽 공백 제거된 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::rtrim(const char* delim)
{
	m_str.erase(m_str.find_last_not_of(delim)+1); 

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::ltrim(const char* delim)
/// @brief  왼쪽 공백 제거
/// @param  delim   [in] 구분자
/// @return 왼쪽 공백 제거된 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::ltrim(const char* delim)
{
	m_str.erase(0, m_str.find_first_not_of(delim));

	return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::replace(const string &search, const string &replace)
/// @brief  문자열 치환
/// @param  search   [in] 찾을 문자열
/// @param	replace	 [in] 변경할 문자열
/// @return 치환된 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::replace(const string &search, const string &replace)
{
    size_t pos = 0;
	
    while ((pos = m_str.find(search, pos)) != string::npos) {
        m_str.replace(pos, search.length(), replace);
        pos += replace.length();
    }

	return *this;
}


//-----------------------------------------------------------------------------
// String operation
//-----------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
/// @fn int jstring::compare(const jstring& cstr)
/// @brief	Compare strings
/// @param  cstr	[in] 비교할 문자열
/// @return 비교 결과 (0: 같음, 음수: 원본 문자열이 작음, 양수: 원본 문자열이 큼)
///////////////////////////////////////////////////////////////////////////////
int jstring::compare(const jstring& cstr)
{
	return strcmp(m_str.c_str(), cstr.c_str());
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int jstring::compare(const jstring& cstr, size_t n)
/// @brief	Compare strings
/// @param  cstr	[in] 비교할 문자열
/// @param  n		[in] 비교할 길이
/// @return 비교 결과 (0: 같음, 음수: 원본 문자열이 작음, 양수: 원본 문자열이 큼)
///////////////////////////////////////////////////////////////////////////////
int jstring::compare(const jstring& cstr, size_t n)
{
	return strncmp(m_str.c_str(), cstr.c_str(), n);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int jstring::casecompare(const jstring& cstr)
/// @brief	Compare strings (ignore case)
/// @param  cstr		[in] 비교할 문자열
/// @return 비교 결과 (0: 같음, 음수: 원본 문자열이 작음, 양수: 원본 문자열이 큼)
///////////////////////////////////////////////////////////////////////////////
int jstring::casecompare(const jstring& cstr)
{
#if defined(_WIN32)
	return _stricmp(m_str.c_str(), cstr.c_str());
#else
	return strcasecmp(m_str.c_str(), cstr.c_str());
#endif
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int jstring::casecompare(const jstring& cstr, size_t n)
/// @brief	Compare strings (ignore case)
/// @param  cstr	[in] 비교할 문자열
/// @param  n		[in] 비교할 길이
/// @return 비교 결과 (0: 같음, 음수: 원본 문자열이 작음, 양수: 원본 문자열이 큼)
///////////////////////////////////////////////////////////////////////////////
int jstring::casecompare(const jstring& cstr, size_t n)
{
#if defined(_WIN32)
	return _strnicmp(m_str.c_str(), cstr.c_str(), n);
#else
	return strncasecmp(m_str.c_str(), cstr.c_str(), n);
#endif
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int jstring::strtoi()
/// @brief	Convert string to integer
/// @return integer
///////////////////////////////////////////////////////////////////////////////
int jstring::strtoi(int base)
{
	return (int)::strtol(m_str.c_str(), NULL, base);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn float jstring::strtof()
/// @brief	Convert string to float
/// @return float
///////////////////////////////////////////////////////////////////////////////
float jstring::strtof()
{
	return (float)::strtod(m_str.c_str(), NULL);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn double jstring::strtod()
/// @brief	Convert string to double
/// @return double
///////////////////////////////////////////////////////////////////////////////
double jstring::strtod()
{
	return ::strtod(m_str.c_str(), NULL);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn long jstring::strtol()
/// @brief	Convert string to long integer
/// @return long
///////////////////////////////////////////////////////////////////////////////
long jstring::strtol(int base)
{
	return ::strtol(m_str.c_str(), NULL, base);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn unsigned long jstring::strtoul()
/// @brief	Convert string to unsigned long integer
/// @return unsigned long integer
///////////////////////////////////////////////////////////////////////////////
unsigned long jstring::strtoul(int base)
{
	return ::strtoul(m_str.c_str(), NULL, base);
}


//-----------------------------------------------------------------------------
// 연산자 오버로딩
//-----------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::operator= (const jstring& cstr)
/// @brief	연산자(=) 오버로딩
/// @param	cstr	[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::operator= (const jstring& cstr)
{
	if (&cstr == this) return (*this);

	this->self().assign(cstr.self());
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::operator= (const string& str)
/// @brief	연산자(=) 오버로딩
/// @param	str		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::operator= (const string& str)
{
	this->self().assign(str);
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::operator= (const char* s)
/// @brief	연산자(=) 오버로딩
/// @param	s		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::operator= (const char* s)
{
	this->self().assign(s);
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::operator= (char c)
/// @brief	연산자(=) 오버로딩
/// @param	c		[in] 문자
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::operator= (char c)
{
	this->self().assign(1, c);
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::operator+= (const jstring& cstr)
/// @brief	연산자(+=) 오버로딩
/// @param	cstr	[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::operator+= (const jstring& cstr)
{
	this->self().append(cstr.self());
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::operator+= (const string& str)
/// @brief	연산자(+=) 오버로딩
/// @param	str		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::operator+= (const string& str)
{
	this->self().append(str);
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::operator+= (const char* s)
/// @brief	연산자(+=) 오버로딩
/// @param	s		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::operator+= (const char* s)
{
	this->self().append(s);
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring& jstring::operator+= (const char c)
/// @brief	연산자(+=) 오버로딩
/// @param	c		[in] 문자
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring& jstring::operator+= (const char c)
{
	this->self().append(1, c);
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring operator+ (jstring& lhs, const jstring& rhs)
/// @brief	연산자(+) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring operator+ (jstring& lhs, const jstring& rhs)
{
	lhs += rhs; // reuse compound assignment
	
	return lhs; // return the result by value (uses move constructor)
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring operator+ (const jstring& lhs, const string& rhs)
/// @brief	연산자(+) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring operator+ (jstring& lhs, const string& rhs)
{
	lhs += rhs; // reuse compound assignment
	
	return lhs; // return the result by value (uses move constructor)
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring operator+ (const jstring& lhs, const char* rhs)
/// @brief	연산자(+) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring operator+ (jstring& lhs, const char* rhs)
{
	lhs += rhs; // reuse compound assignment
	
	return lhs; // return the result by value (uses move constructor)
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring operator+ (const jstring& lhs, const char rhs)
/// @brief	연산자(+) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring operator+ (jstring& lhs, const char rhs)
{
	lhs += rhs; // reuse compound assignment
	
	return lhs; // return the result by value (uses move constructor)
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring operator+ (const string& lhs, const jstring& rhs)
/// @brief	연산자(+) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring operator+ (const string& lhs, const jstring& rhs)
{
	jstring tmp(lhs);

	tmp += rhs;
	return (tmp);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring operator+ (const char* lhs, const jstring& rhs)
/// @brief	연산자(+) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring operator+ (const char* lhs, const jstring& rhs)
{
	jstring tmp(lhs);
	
	tmp += rhs;
	return (tmp);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn jstring operator+ (const char lhs, const jstring& rhs)
/// @brief	연산자(+) 오버로딩
/// @param	lhs		[in] 문자
/// @param	rhs		[in] 문자열
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
jstring operator+ (const char lhs, const jstring& rhs)
{
	jstring tmp(1, lhs);
	
	tmp += rhs;
	return (tmp);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool operator< (const jstring& lhs, const jstring& rhs)
/// @brief	연산자(<) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 클래스 비교 결과
///////////////////////////////////////////////////////////////////////////////
bool operator< (const jstring& lhs, const jstring& rhs)
{
	return (lhs.self() < rhs.self());
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool operator> (const jstring& lhs, const jstring& rhs)
/// @brief	연산자(>) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 클래스 비교 결과
///////////////////////////////////////////////////////////////////////////////
bool operator> (const jstring& lhs, const jstring& rhs)
{
	return (lhs.self() > rhs.self());
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool operator<= (const jstring& lhs, const jstring& rhs)
/// @brief	연산자(<=) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 클래스 비교 결과
///////////////////////////////////////////////////////////////////////////////
bool operator<= (const jstring& lhs, const jstring& rhs)
{
	return !(lhs > rhs);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool operator>= (const jstring& lhs, const jstring& rhs)
/// @brief	연산자(>=) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 클래스 비교 결과
///////////////////////////////////////////////////////////////////////////////
bool operator>= (const jstring& lhs, const jstring& rhs)
{
	return !(lhs < rhs); 
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool operator== (const jstring& lhs, const jstring& rhs)
/// @brief	연산자(==) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 클래스 비교 결과
///////////////////////////////////////////////////////////////////////////////
bool operator== (const jstring& lhs, const jstring& rhs)
{
	return (lhs.self() == rhs.self()); 
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool operator!= (const jstring& lhs, const jstring& rhs)
/// @brief	연산자(==) 오버로딩
/// @param	lhs		[in] 문자열
/// @param	rhs		[in] 문자열
/// @return 클래스 비교 결과
///////////////////////////////////////////////////////////////////////////////
bool operator!= (const jstring& lhs, const jstring& rhs)
{
	return (lhs.self() != rhs.self()); 
}


__END_NAMESPACE_JFX
