// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       bstring.cpp
/// @brief      bstring Ŭ���� ����
/// @author     Jongkap Jeong <jongkap@mail.com>
/// @date       2008/11/13 - ó���ۼ�
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#include "jfx.h"

#include <string.h>
#include <stdarg.h>

__BEGIN_NAMESPACE_JFX

#define IO_BUF_MAX 1024  ///< Ȯ���� IO ���� ũ��

namespace
{
	byte_t NULL_VAL = 0x00;
}

bstring::bstring()
{
	m_bstr = 0;
	m_size = 0;
	m_max_size = 0;
	m_offset = 0;
}

bstring::bstring(byte_t* bstr, size_t size)
{
	m_bstr = 0;
	m_size = 0;
	m_max_size = 0;
	m_offset = 0;
	
	bstrcpy(bstr, size);
}

bstring::bstring(const char* cstr)
{
	m_bstr = 0;
	m_size = 0;
	m_max_size = 0;
	m_offset = 0;
	
	bstrcpy(cstr);
}

bstring::bstring(const bstring& rhs)
{
	m_bstr = 0;
	m_size = 0;
	m_max_size = 0;
	m_offset = 0;
	
	bstrcpy(rhs.data(), rhs.size());
}

bstring::~bstring()
{
	if (m_bstr) free();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool bstring::empty()
/// @brief	��Ʈ�� ������� ����
/// @return ������ ����: ture, ������ ����: false
///////////////////////////////////////////////////////////////////////////////
bool bstring::empty()
{
	if ((m_bstr == 0) || (m_size == 0)) return true;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool bstring::end()
/// @brief	�������� ���̿� ������ ����
/// @return ����: ture, ����: false
///////////////////////////////////////////////////////////////////////////////
bool bstring::end()
{
	if (m_offset >= m_size) return true;
	return false;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn size_t& bstring::offset()
/// @brief	���� ������ ��ġ
/// @return	������
///////////////////////////////////////////////////////////////////////////////
size_t bstring::offset()
{
	return m_offset;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::setsize(size_t n)
/// @brief	��Ʈ�� ���� ����
/// @param	n	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::setsize(size_t n)
{
	m_size = n;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::setaddsize(size_t n)
/// @brief	��Ʈ�� �߰� ���� ����
/// @param	n	[in] �߰� ����
///////////////////////////////////////////////////////////////////////////////
void bstring::setaddsize(size_t n)
{
	m_size += n;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::setoffset(size_t n)
/// @brief	��Ʈ�� ������ ����
/// @param	n	[in] ������
///////////////////////////////////////////////////////////////////////////////
void bstring::setoffset(size_t n)
{
	m_offset = n;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::seek(size_t n)
/// @brief	��Ʈ�� ������ ����
/// @param	n	[in] ������
///////////////////////////////////////////////////////////////////////////////
void bstring::seek(size_t n)
{
	setoffset(n);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn size_t bstring::size() const
/// @brief	��Ʈ�� ����
/// @return	����
///////////////////////////////////////////////////////////////////////////////
size_t bstring::size() const
{
	return m_size;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn size_t& bstring::size()
/// @brief	��Ʈ�� ����
/// @return	����
///////////////////////////////////////////////////////////////////////////////
size_t& bstring::size()
{
	return m_size;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn size_t bstring::length() const
/// @brief	��Ʈ�� ����
/// @return	����
///////////////////////////////////////////////////////////////////////////////
size_t bstring::length() const
{
	return size();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn size_t& bstring::length()
/// @brief	��Ʈ�� ����
/// @return	����
///////////////////////////////////////////////////////////////////////////////
size_t& bstring::length()
{
	return size();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn size_t bstring::max_size() const
/// @brief	�ִ� ���� ũ��
/// @return	����
///////////////////////////////////////////////////////////////////////////////
size_t bstring::max_size() const
{
	return m_max_size;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn size_t& bstring::max_size()
/// @brief	�ִ� ���� ũ��
/// @return	����
///////////////////////////////////////////////////////////////////////////////
size_t& bstring::max_size()
{
	return m_max_size;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t bstring::at(size_t n) const
/// @brief	n��° ���� (zero-based)
/// @param	n	[in] �ε���
/// @return ������
///////////////////////////////////////////////////////////////////////////////
byte_t bstring::at(size_t n) const
{
	if ((n < 0) || (n >= size())) return NULL_VAL;
	return m_bstr[n];
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t& bstring::at(size_t n)
/// @brief	n��° ���� (zero-based)
/// @param	n	[in] �ε���
/// @return ������
///////////////////////////////////////////////////////////////////////////////
byte_t& bstring::at(size_t n)
{
	if ((n < 0) || (n >= size())) return NULL_VAL;
	return m_bstr[n];
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t bstring::next()
/// @brief	���� ������ ��ȯ �� ����
/// @return ������
///////////////////////////////////////////////////////////////////////////////
byte_t bstring::next()
{
	byte_t data = m_bstr[m_offset++];
	
	if (m_offset > m_size) m_offset = m_size;
	return data;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t bstring::previous()
/// @brief	���� ������ ��ȯ �� ����
/// @return ������
///////////////////////////////////////////////////////////////////////////////
byte_t bstring::previous()
{
	byte_t data = m_bstr[m_offset--];
	
	if (m_offset < 0) m_offset = 0;
	return data;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn const char* bstring::c_str() const
/// @brief	��Ʈ�� const char*
/// @return ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
const char* bstring::c_str() const
{
	if (m_bstr) return (const char*)m_bstr;
	return (const char*)"";
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t* bstring::data() const
/// @brief	������ byte_t*
/// @return ������
///////////////////////////////////////////////////////////////////////////////
byte_t* bstring::data() const
{
	return m_bstr;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t*& bstring::data()
/// @brief	������ byte_t*
/// @return ������
///////////////////////////////////////////////////////////////////////////////
byte_t*& bstring::data()
{
	return m_bstr;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t*& bstring::dataptr()
/// @brief	���� ������
/// @return ������ ������
///////////////////////////////////////////////////////////////////////////////
byte_t*& bstring::dataptr()
{
	return m_bstr;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn size_t& bstring::sizeptr()
/// @brief	���� ���� ������
/// @return ���� ������
///////////////////////////////////////////////////////////////////////////////
size_t* bstring::sizeptr()
{
	return &m_size;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn bool bstring::writable(size_t n)
/// @brief	������ ���� �ִ��� ����
/// @param	n	[in] ����
/// @return ���� ����: ture, ���� ����: false
///////////////////////////////////////////////////////////////////////////////
bool bstring::writable(size_t n)
{
	if ((m_max_size - m_size) > n) return true;
	return false;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::free()
/// @brief	�Ҵ� �޸� ����
///////////////////////////////////////////////////////////////////////////////
void bstring::free()
{
	if (m_bstr) 
	{
		clear();
		delete[] m_bstr;
	}

	m_bstr = 0;
	m_size = 0;
	m_max_size = 0;
	m_offset = 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::clear()
/// @brief	���� �ʱ�ȭ
///////////////////////////////////////////////////////////////////////////////
void bstring::clear()
{
	if (m_bstr == 0) return;
		
	memset(m_bstr, 0, m_max_size+1);
	m_size = 0;
	m_offset = 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::allocation(size_t n)
/// @brief	���� �Ҵ�
/// @param	n	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::allocation(size_t n)
{
	if (m_bstr && (m_max_size < n)) free();
	if (m_bstr == 0) reallocation(n);
	clear();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::alloc(size_t n)
/// @brief	���� �Ҵ�
/// @param	n	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::alloc(size_t n)
{
	allocation(n);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::malloc(size_t n)
/// @brief	���� �Ҵ�
/// @param	n	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::malloc(size_t n)
{
	allocation(n);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::malloc_and_size(size_t n)
/// @brief	���� �Ҵ� �� ���� ũ�� ����
/// @param	n	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::malloc_and_setsize(size_t n)
{
	malloc(n);
	m_size = n;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::reallocation(size_t n)
/// @brief	���� �� �Ҵ�
/// @param	n	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::reallocation(size_t n)
{
	byte_t* bk_str = 0;
	
	// ���� ���۷� ����ϸ� �� �Ҵ� ���� �ʴ´�.
	if (m_bstr && (m_max_size >= n)) return;

	// ���� ���� ���
	if (m_bstr && (m_size > 0))
	{
		bk_str = new byte_t[m_size];
		memset(bk_str, 0, m_size);
		memcpy(bk_str, m_bstr, m_size);

	}
	
	// �߰��� �ִ� ���� ũ�� ���ϱ�
	while (true)
	{
		m_max_size += IO_BUF_MAX;
		if (m_max_size >= (m_size + n)) break;
	}
	
	if (m_bstr) delete[] m_bstr;
	m_bstr = new byte_t[m_max_size+1];
	memset(m_bstr, 0, m_max_size+1);

	// ���� ���� ���� ����
	if (bk_str)
	{
		memcpy(m_bstr, bk_str, m_size);
		memset(bk_str, 0, m_size);
		delete[] bk_str;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::realloc(size_t n)
/// @brief	���� �� �Ҵ�
/// @param	n	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::realloc(size_t n)
{
	reallocation(n);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::bstrcpy(const byte_t* bstr, size_t n)
/// @brief	��Ʈ�� ���ۿ� bstr ����
/// @param	bstr	[in] ����
/// @param	n		[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::bstrcpy(const byte_t* bstr, size_t n)
{
	allocation(n);
	append(bstr, n);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::bstrcpy(const char* cstr)
/// @brief	��Ʈ�� ���ۿ� cstr ����
/// @param	cstr	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::bstrcpy(const char* cstr)
{
	bstrcpy((byte_t*)cstr, strlen(cstr));
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::append(const byte_t* bstr, size_t n)
/// @brief	��Ʈ�� ���ۿ� bstr �߰�
/// @param	bstr	[in] ����
/// @param	n		[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::append(const byte_t* bstr, size_t n)
{
	if (!writable(n)) reallocation(n);

	memcpy(m_bstr+m_size, bstr, n);
	m_size += n;
	m_bstr[m_size] = '\0';
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::append(const char* cstr)
/// @brief	��Ʈ�� ���ۿ� cstr �߰�
/// @param	cstr	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::append(const char* cstr)
{
	append((byte_t*)cstr, strlen(cstr));
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::format(const char* cstr)
/// @brief	��Ʈ�� ���۸� ���� ��Ʈ������ ����
/// @param  fmt         [in] ����
/// @param  ...         [in] �μ�����Ʈ
///////////////////////////////////////////////////////////////////////////////
void bstring::format(const char* fmt, ...)
{
    va_list args;
    int n, size = 1024;
    byte_t* value = 0;     
    
    if (fmt == 0) return;
	free();

    while (true)
    {
		if (value) 
		{			
			delete[] value;
			value = 0;
		}
        value = new byte_t[size+1];
        if (value == 0) return;
        
        memset(value, 0, size+1);
        
        va_start(args, fmt); 
        n = vsnprintf((char*)value, size, fmt, args);
        va_end(args);
        
        if ((n > -1) && (n < size))
        {
            value[n] = '\0';

            bstrcpy(value, n);
			memset(value, 0, n);
			delete[] value;
            break;
        }
        
        size *= 2;
    }
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::replace(size_t pos, size_t len, const byte_t* bstr, size_t n)
/// @brief	��Ʈ�� ������ pos ��ġ�� bstr�� ��ü
/// @param	pos		[in] ��ü�� ��ġ
/// @param	len		[in] ��ü�� ����
/// @param	bstr	[in] ����
/// @param	n		[in] ���� ����
///////////////////////////////////////////////////////////////////////////////
void bstring::replace(size_t pos, size_t len,
		const byte_t* bstr, size_t n)
{
	if (len <= n) memcpy(m_bstr+pos, bstr, len);
	else
	{
		memset(m_bstr+pos, 0, len);
		memcpy(m_bstr+pos, bstr, n);
	}
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::replace(size_t pos, size_t len, const char* cstr)
/// @brief	��Ʈ�� ������ pos ��ġ�� cstr�� ��ü
/// @param	pos		[in] ��ü�� ��ġ
/// @param	len		[in] ��ü�� ����
/// @param	cstr	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::replace(size_t pos, size_t len, const char* cstr)
{
	if (len <= strlen(cstr)) memcpy(m_bstr+pos, cstr, len);
	else
	{
		memset(m_bstr+pos, 0, len);
		memcpy(m_bstr+pos, cstr, strlen(cstr));
	}
}


///////////////////////////////////////////////////////////////////////////////
/// @fn size_t bstring::read(byte_t* bstr, size_t n)
/// @brief	��Ʈ�� ���ۿ��� n���̸�ŭ ������ �б�
/// @param	bstr	[out] ����
/// @param	n		[in] ���� ����
/// @return ���� ���� (����: 0, ����: ���� byte�� - ���� ��ŭ offset �̵�)
///////////////////////////////////////////////////////////////////////////////
size_t bstring::read(byte_t* bstr, size_t n)
{
   	size_t nread = n;
   	
   	// ���� �����÷ο� ����
	if (m_size < (m_offset + n)) nread = m_size - m_offset; 
	    
	memcpy(bstr, m_bstr+m_offset, nread);	
	m_offset += nread;
	
	return nread;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::push(byte_t b)
/// @brief	��Ʈ�� ���� ���� b �߰�
/// @param	b	[in] ����
///////////////////////////////////////////////////////////////////////////////
void bstring::push(byte_t b)
{
	append(&b, 1);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::pop()
/// @brief	��Ʈ�� ���� �� ����
///////////////////////////////////////////////////////////////////////////////
void bstring::pop()
{
	if (m_size > 0) m_bstr[--m_size] = '\0';
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void bstring::erase(size_t n, int whence)
/// @brief	���� ����
/// @param	n		[in] ������ ����
/// @param	whence	[in] ���� ��� (0: ó������, 1: ���� ��ġ, 2: ������ ����)
///////////////////////////////////////////////////////////////////////////////
void bstring::erase(size_t n, int whence)
{
	// ���ڿ� ó�� �������� ����
	if ((whence & BSTR_SET) == BSTR_SET)
	{
		size_t offset = n;
		
		// trim�� �� ��쿡�� ��, ����, CRLF ���� ����
		if ((whence & BSTR_TRIM) == BSTR_TRIM)
		{
			for (size_t i = offset ; i < m_size; i++) 
			{
				if ((m_bstr[i] == '\t') || (m_bstr[i] == ' ') || 
					(m_bstr[i] == '\r') || (m_bstr[i] == '\n')) ++offset;
				else break;
			}
		}
		
		if (offset > m_size) offset = m_size;
		
		for (size_t i = offset ; i < m_size; i++) 
			m_bstr[i-offset] = m_bstr[i];
		
		m_size -= offset;
		m_bstr[m_size] = '\0';
		
		m_offset = 0;
	}
	// ���� ���ڿ� �������� ����
	else if ((whence & BSTR_CUR) == BSTR_CUR)
	{
		// �ƹ� ���� ����
	}
	// ���ڿ� ������ �������� ����
	else if ((whence & BSTR_END) == BSTR_END)
	{
		size_t offset = m_size - n;
		
		// trim�� �� ��쿡�� ��, ����, CRLF ���� ����
		if ((whence & BSTR_TRIM) == BSTR_TRIM)
		{
			for (size_t i = offset-1; i >= 0; --i)
			{
				if ((m_bstr[i] == '\t') || (m_bstr[i] == ' ') || 
					(m_bstr[i] == '\r') || (m_bstr[i] == '\n')) --offset;
				else break;
			}
		}
		
		if (offset < 0) offset = 0;
		
		m_size = offset;
		m_bstr[m_size] = '\0';
		
		m_offset = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring bstring::operator+ (const bstring& rhs)
/// @brief	������(+) �����ε�
/// @param	rhs		[in] bstr ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
bstring bstring::operator+ (const bstring& rhs)
{
	bstring tmp(*this);
	
	tmp.append(rhs.data(), rhs.size());
	return (tmp);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring bstring::operator+ (const char* cstr)
/// @brief	������(+) �����ε�
/// @param	cstr	[in] ����
///////////////////////////////////////////////////////////////////////////////
bstring bstring::operator+ (const char* cstr)
{
	bstring tmp(*this);
	
	tmp.append(cstr);
	return (tmp);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring operator+ (const bstring& lhs, const bstring& rhs)
/// @brief	������(+) �����ε�
/// @param	lhs		[in] bstr ��Ʈ��
/// @param	rhs		[in] bstr ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
bstring operator+ (const bstring& lhs, const bstring& rhs)
{
	bstring tmp(lhs);
	
	tmp.append(rhs.data(), rhs.size());
	return (tmp);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring operator+ (const bstring& lhs, const char* rhs)
/// @brief	������(+) �����ε�
/// @param	lhs		[in] bstr ��Ʈ��
/// @param	rhs		[in] cstr ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
bstring operator+ (const bstring& lhs, const char* rhs)
{
	bstring tmp(lhs);
	
	tmp.append(rhs);
	return (tmp);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring operator+ (const char* lhs, const bstring& rhs)
/// @brief	������(+) �����ε�
/// @param	lhs		[in] cstr ��Ʈ��
/// @param	rhs		[in] bstr ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
bstring operator+ (const char* lhs, const bstring& rhs)
{
	bstring tmp(lhs);
	
	tmp.append(rhs.data(), rhs.size());
	return (tmp);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring bstring::operator+= (const bstring & rhs)
/// @brief	������(+=) �����ε�
/// @param	rhs		[in] bstr ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
bstring& bstring::operator+= (const bstring & rhs)
{
	if (&rhs == this) return (*this);

	this->append(rhs.data(), rhs.size());
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring bstring::operator+= (const char* cstr)
/// @brief	������(+=) �����ε�
/// @param	cstr	[in] ����
///////////////////////////////////////////////////////////////////////////////
bstring& bstring::operator+= (const char* cstr)
{
	this->append(cstr);
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring& bstring::operator= (const bstring& rhs)
/// @brief	������(=) �����ε�
/// @param	rhs		[in] bstr ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
bstring& bstring::operator= (const bstring& rhs)
{
	if (&rhs == this) return (*this);
	
	this->bstrcpy(rhs.data(), rhs.size());
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring& bstring::operator= (const char* cstr)
/// @brief	������(=) �����ε�
/// @param	cstr	[in] ����
///////////////////////////////////////////////////////////////////////////////
bstring& bstring::operator= (const char* cstr)
{
	this->bstrcpy(cstr);
	return (*this);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t bstring::operator[] (size_t n) const
/// @brief	������(=) �����ε�
/// @param	n	[in] �ε���
///////////////////////////////////////////////////////////////////////////////
byte_t bstring::operator[] (size_t n) const
{
	return at(n);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn byte_t& bstring::operator[] (size_t n)
/// @brief	������(=) �����ε�
/// @param	n	[in] �ε���
///////////////////////////////////////////////////////////////////////////////
byte_t& bstring::operator[] (size_t n)
{
	return at(n);
}

__END_NAMESPACE_JFX
