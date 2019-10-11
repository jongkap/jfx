// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       StrUtil.cpp
/// @brief      StrUtil 클래스 구현
/// @author     Jongkap Jeong <jongkap@mail.com>
/// @date       2015/10/01 - First Release
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#include "jfx.h"
#include <string.h>
#include <stdarg.h>

__BEGIN_NAMESPACE_JFX

///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::toUpper(char *string)
/// @brief  대문자 변환
/// @param	string		[in/out] 원본 문자열
///////////////////////////////////////////////////////////////////////////////
void StrUtil::toUpper(char *string)
{
    char* buf = string;
    while (*buf != 0) *string++ = toupper(*buf++);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::toLower(char *string)
/// @brief  소문자 변환
/// @param	string		[in/out] 원본 문자열
///////////////////////////////////////////////////////////////////////////////
void StrUtil::toLower(char *string)
{
    char* buf = string;
    while (*buf != 0) *string++ = tolower(*buf++);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn const char* StrUtil::strError(int errnum)
/// @brief	숫자 타입을 스트링으로 변환
/// @param	num		[in] 숫자
/// @return 문자열
///////////////////////////////////////////////////////////////////////////////
const char* StrUtil::strError(int errnum)
{
	jstring out;

#if defined(_WIN32)
	char* lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		errnum, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);

	out.self().assign(lpMsgBuf);
	LocalFree(lpMsgBuf);
#else
	char msg_buff[1024+1] = {0, };
	strerror_r(errnum, msg_buff, 1024);
	out.self().assign(msg_buff);
#endif

	out.trim();
	return out.c_str();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn char* StrUtil::strToken(register char **stringp, register const char *delim)
/// @brief  스트링 토큰나이저
/// @note   사용법:
///             char str[256];
///             strcpy(str, "a;;c");
///             char* buf = str;
///             char* pos;
///
///             pos = strToken(&buf, ";");      // pos = a
///             if (pos != NULL) ...
///             pos = strToken(&buf, ";");      // pos = ""
///             if (pos != NULL) ...
///             pos = strToken(&buf, ";");      // pos = c
///             if (pos != NULL) ...
///
///             --------------------------------------------
///
///             pos = strToken(&buf, ";");
///             while(pos != NULL) {
///                 pos = strToken(&buf, ";");
///             }
/// @param  stringp [in/out] 스트링
/// @param  delim   [in] 구분자
///////////////////////////////////////////////////////////////////////////////
char* StrUtil::strToken(register char **stringp, register const char *delim)
{
    register char *s;
    register const char *spanp;
    register int c, sc;
    char *tok;

    if ((s = *stringp) == NULL)
        return (NULL);

    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::putInt16u(void *vp, uint_16t v)
/// @brief  16bit 값 설정
/// @param  vp  [out] 저장할 변수
/// @param  v   [in] 16bit 정수 값
///////////////////////////////////////////////////////////////////////////////
void StrUtil::putInt16u(void *vp, uint_16t v)
{
    byte_t *p = (byte_t *)vp;

    p[0] = (byte_t)(v >> 8) & 0xff;
    p[1] = (byte_t)v & 0xff;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn uint_16t StrUtil::getInt16u(const void *vp)
/// @brief  16bit 값 읽기
/// @param  vp  [in] 저장된 변수
/// @return 16bit 정수 값
///////////////////////////////////////////////////////////////////////////////
uint_16t StrUtil::getInt16u(const void *vp)
{
    const byte_t *p = (const byte_t *)vp;
    uint_16t v;

    v =  (uint_16t)p[0] << 8;
    v |= (uint_16t)p[1];

    return (v);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::putInt32u(void *vp, uint_32t v)
/// @brief  32bit 값 설정
/// @param  vp  [out] 저장할 변수
/// @param  v   [in] 32bit 정수 값
///////////////////////////////////////////////////////////////////////////////
void StrUtil::putInt32u(void *vp, uint_32t v)
{
    byte_t *p = (byte_t *)vp;

    p[0] = (byte_t)(v >> 24) & 0xff;
    p[1] = (byte_t)(v >> 16) & 0xff;
    p[2] = (byte_t)(v >> 8) & 0xff;
    p[3] = (byte_t)v & 0xff;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn uint_32t StrUtil::getInt32u(const void *vp)
/// @brief  32bit 값 읽기
/// @param  vp  [in] 저장된 변수
/// @return 32bit 정수 값
///////////////////////////////////////////////////////////////////////////////
uint_32t StrUtil::getInt32u(const void *vp)
{
    const byte_t *p = (const byte_t *)vp;
    uint_32t v;

    v  = (uint_32t)p[0] << 24;
    v |= (uint_32t)p[1] << 16;
    v |= (uint_32t)p[2] << 8;
    v |= (uint_32t)p[3];

    return (v);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::putInt64u(void *vp, uint_32t v)
/// @brief  64bit 값 설정
/// @param  vp  [out] 저장할 변수
/// @param  v   [in] 64bit 정수 값
///////////////////////////////////////////////////////////////////////////////
void StrUtil::putInt64u(void *vp, uint_64t v)
{
    byte_t *p = (byte_t *)vp;

    p[0] = (byte_t)(v >> 56) & 0xff;
    p[1] = (byte_t)(v >> 48) & 0xff;
    p[2] = (byte_t)(v >> 40) & 0xff;
    p[3] = (byte_t)(v >> 32) & 0xff;
    p[4] = (byte_t)(v >> 24) & 0xff;
    p[5] = (byte_t)(v >> 16) & 0xff;
    p[6] = (byte_t)(v >> 8) & 0xff;
    p[7] = (byte_t)v & 0xff;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn uint_64t StrUtil::getInt64u(const void *vp)
/// @brief  64bit 값 읽기
/// @param  vp  [in] 저장된 변수
/// @return 64bit 정수 값
///////////////////////////////////////////////////////////////////////////////
uint_64t StrUtil::getInt64u(const void *vp)
{
    const byte_t *p = (const byte_t *)vp;
    uint_64t v;

    v  = (uint_64t)p[0] << 56;
    v |= (uint_64t)p[1] << 48;
    v |= (uint_64t)p[2] << 40;
    v |= (uint_64t)p[3] << 32;
    v |= (uint_64t)p[4] << 24;
    v |= (uint_64t)p[5] << 16;
    v |= (uint_64t)p[6] << 8;
    v |= (uint_64t)p[7];

    return (v);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::binToHex(string* out, const unsigned char* data, size_t len)
/// @brief  Bin을 Hex로 변환
/// @param  out     [out] 저장할 스트링
/// @param  data    [in] 바이너리 데이터
/// @param  len     [in] 길이
///////////////////////////////////////////////////////////////////////////////
void StrUtil::binToHex(string* out, const unsigned char* data, size_t len)
{
    if (out == 0) return;
    if (data == 0 || len == 0) return;

    char buf[4];
    for (size_t i = 0; i < len; i++)
    {
        sprintf(buf, "%02x", data[i]);
        *out += buf;
    }
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::hexToBin(unsigned char* data, size_t* len, const string& in)
/// @brief  Hex를 Bin으로 변환
/// @note   data의 길이는 in/2이며, null terminate 영역을 1byte 더 추가한다.\n
///         data를 메모리를 할당받지 않고 실행한 경우 자동으로 메모리가 할당되므로.\n
///         호출한 쪽에서 반드시 delete[] data로 메모리를 해제해줘야 한다.
/// @param  data    [out] 바이너리 데이터 (in/2+1)
/// @param  len     [out] 길이
/// @param  in      [in] 저장할 스트링
///////////////////////////////////////////////////////////////////////////////
void StrUtil::hexToBin(unsigned char* data, size_t* len, const string& in)
{
    char* pos = (char*)in.c_str();
    *len = in.size()/2;

    if (data == 0) return;
    memset(data, 0, in.size()+1);

    size_t idx = 0;
    char temp[5] = {'0', 'x', 0, };
    int nHexPos = 0;

    //----------------------------------------------------
    // 공백 또는 ':' 이 포함된 hex string 변환
    //----------------------------------------------------
    while (*pos != 0) {
        if ((*pos != ' ') && (*pos != ':'))
        {
            temp[2+nHexPos] = *pos;
            ++nHexPos;
            if (nHexPos >= 2)
            {
                data[idx++] = (unsigned char)strtoul(temp, 0, 16);
                nHexPos = 0;
            }
        }
        ++pos;
    }

    *len = idx;

    //----------------------------------------------------
    // 공백없이 저장된 hex string 변환
    //----------------------------------------------------
    // for (int i=0; i<*len; i++)
    // {
    //  memcpy(temp+2, pos+i*2, 2);
    //  (*data)[i] = (unsigned char)strtoul(temp, 0, 16);
    // }
    //----------------------------------------------------
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::hexDump(string* out, const char* prefix, unsigned char* data, size_t len)
/// @brief  Hex로 출력
/// @param  out     [out] 저장할 스트링
/// @param  prefix  [in] 프리픽스
/// @param  data    [in] 바이너리 데이터
/// @param  len     [in] 길이
///////////////////////////////////////////////////////////////////////////////
void StrUtil::hexDump(string* out, const char* prefix, unsigned char* data, size_t len)
{
    int addr, cnt, m, n, p, q;
    unsigned char *b;
    char buf[1024+1];

    if (out == 0) return;

    if (data == NULL || len == 0) 
	{
		if (prefix) *out = prefix;
        *out += "[Empty Structure]\n";
	}
    else
    {
        (*out).clear();
        if (prefix) *out += prefix;
		*out +="Address                   Hexadecimal values                   Printable";
		*out +="\n";
		if (prefix) *out += prefix;
        *out +="--------  ------------------------------------------------  ----------------";

        cnt = 0;
        addr = 0;
        p = (int)len/16;
        q = (int)len%16;

        while ( cnt <= p ) {
            b = data + 16*cnt;

            if ( cnt < p || (cnt == p && q > 0 ) ) {
				if (prefix) sprintf(buf, "\n%s%08lx  ", prefix, (unsigned long)addr);
				else sprintf(buf, "\n%08lx  ", (unsigned long)addr);
                *out += buf;
                addr = addr + 16;
            }

            for ( m = 0; m < 16; m++ ) {
                if ( cnt < p || (cnt == p && m < q ) ) {
                    sprintf(buf, "%02x", (unsigned char)*(b+m));
                    *out += buf;
                }
                else {
                    sprintf(buf, "  ");
                    *out += buf;
                }
                sprintf(buf, " ");
                *out += buf;

                if (m == 7) {
                    sprintf(buf, " ");
                    *out += buf;
                }
            }

            sprintf(buf, " " );
            *out += buf;
            for ( n = 0; n < 16; n++ ) {
                if ( cnt < p || (cnt == p && n < q ) ) {
                    if ( ( *(b+n) < 32 ) || ( *(b+n) > 126 ) ) {
                        sprintf(buf, "%c", '.');
                        *out += buf;
                    }
                    else {
                        sprintf(buf, "%c", *(b+n));
                        *out += buf;
                    }
                }
            }
            
            cnt++;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::hexDump(string* out, unsigned char* data, size_t len)
/// @brief  Hex로 출력
/// @param  out     [out] 저장할 스트링
/// @param  data    [in] 바이너리 데이터
/// @param  len     [in] 길이
///////////////////////////////////////////////////////////////////////////////
void StrUtil::hexDump(string* out, unsigned char* data, size_t len)
{
    int addr, cnt, m, n, p, q;
    unsigned char *b;
    char buf[1024+1];

    if (out == 0) return;

    if (data == NULL || len == 0) 
	{
        *out = "[Empty Structure]\n";
	}
    else
    {
        (*out).clear();
        *out +="Address                   Hexadecimal values                   Printable";
        *out +="\n--------  ------------------------------------------------  ----------------";

        cnt = 0;
        addr = 0;
        p = (int)len/16;
        q = (int)len%16;

        while ( cnt <= p ) {
            b = data + 16*cnt;

            if ( cnt < p || (cnt == p && q > 0 ) ) {
                sprintf(buf, "\n%08lx  ", (unsigned long)addr);
                *out += buf;
                addr = addr + 16;
            }

            for ( m = 0; m < 16; m++ ) {
                if ( cnt < p || (cnt == p && m < q ) ) {
                    sprintf(buf, "%02x", (unsigned char)*(b+m));
                    *out += buf;
                }
                else {
                    sprintf(buf, "  ");
                    *out += buf;
                }
                sprintf(buf, " ");
                *out += buf;

                if (m == 7) {
                    sprintf(buf, " ");
                    *out += buf;
                }
            }

            sprintf(buf, " " );
            *out += buf;
            for ( n = 0; n < 16; n++ ) {
                if ( cnt < p || (cnt == p && n < q ) ) {
                    if ( ( *(b+n) < 32 ) || ( *(b+n) > 126 ) ) {
                        sprintf(buf, "%c", '.');
                        *out += buf;
                    }
                    else {
                        sprintf(buf, "%c", *(b+n));
                        *out += buf;
                    }
                }
            }
            
            cnt++;
        }
    }
}


//-----------------------------------------------------------------------------
// Fast base64 functions for encode/decode
// Ref: http://www.codeproject.com/Tips/813146/Fast-base-functions-for-encode-decode
//      http://www.adp-gmbh.ch/cpp/common/base64.html

static const string _base64_chars = 
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

static inline bool _is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}


///////////////////////////////////////////////////////////////////////////////
/// @fn uint_t StrUtil::base64Encode(char* out_str,
///						const byte_t* in_byte_str, 
///						uint_t in_len)
/// @brief	Base64 인코딩
/// @param	out_str			[out] 인코딩 값
/// @param	in_byte_str		[in] 입력 값
/// @param	in_len			[in] 입력 값 길이
/// @return 인코딩 길이
///////////////////////////////////////////////////////////////////////////////
uint_t StrUtil::base64Encode(char* out_str,
						const byte_t* in_byte_str, 
						uint_t in_len)
{
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3] = { 0, 0, 0 };
	unsigned char char_array_4[4] = { 0, 0, 0, 0 };

	int out_len = 0;
	while (in_len--)
	{
		char_array_3[i++] = *(in_byte_str++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; i < 4 ; i++)
			{
				out_str[out_len++] = _base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
		{
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; j < (i + 1); j++)
		{
			out_str[out_len++] = _base64_chars[char_array_4[j]];
		}

		while (i++ < 3)
		{
			out_str[out_len++] = '=';
		}
	}

	return out_len;

}


///////////////////////////////////////////////////////////////////////////////
/// @fn uint_t StrUtil::base64Decode(byte_t* out_byte_str, 
///						uint_t* out_len,
///						const char* in_b64_str)
/// @brief	Base64 디코딩
/// @param	out_byte_str	[out] 디코딩 값
/// @param	out_len			[out] 디코딩 값 길이
/// @param	in_b64_str		[in] 인코딩된 입력 값
/// @return 디코딩 길이
///////////////////////////////////////////////////////////////////////////////
uint_t StrUtil::base64Decode(byte_t* out_byte_str, 
						uint_t* out_len,
						const char* in_b64_str)
{
    size_t i = 0;
    size_t j = 0;
    int in_ = 0;
    unsigned char char_array_3[3] = { 0, 0, 0 };
    unsigned char char_array_4[4] = { 0, 0, 0, 0 };
	
	int in_len = (int)strlen(in_b64_str);

    *out_len = 0;
    while (in_len-- && (in_b64_str[in_] != '=') && _is_base64(in_b64_str[in_]))
    {
        char_array_4[i++] = in_b64_str[in_]; in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
            {
                char_array_4[i] = static_cast<unsigned char>(_base64_chars.find(char_array_4[i]));
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; i < 3; i++)
            {
                out_byte_str[(*out_len)++] = char_array_3[i];
            }
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
        {
            char_array_4[j] = 0;
        }

        for (j = 0; j < 4; j++)
        {
            char_array_4[j] = static_cast<unsigned char>(_base64_chars.find(char_array_4[j]));
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++)
        {
            out_byte_str[(*out_len)++] = char_array_3[j];
        }
    }
    return *out_len;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn uint_t StrUtil::base64EncodeString(string& out_str, const byte_t* in_byte_str, size_t in_len)
/// @brief	Base64 인코딩
/// @param	out_str			[out] 인코딩 값
/// @param	in_byte_str		[in] 입력 값
/// @param	in_len			[in] 입력 값 길이
/// @return 인코딩 길이
///////////////////////////////////////////////////////////////////////////////
uint_t StrUtil::base64EncodeString(string& out_str, const byte_t* in_byte_str, size_t in_len)
{
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3] = { 0, 0, 0 };
	unsigned char char_array_4[4] = { 0, 0, 0, 0 };

	out_str.clear();
	while (in_len--)
	{
		char_array_3[i++] = *(in_byte_str++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; i < 4 ; i++)
			{
				out_str += _base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
		{
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; j < (i + 1); j++)
		{
			out_str += _base64_chars[char_array_4[j]];
		}

		while (i++ < 3)
		{
			out_str += '=';
		}
	}

	return (uint_t)out_str.size();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn uint_t StrUtil::base64DecodeString(byte_t* out_byte_str, size_t& out_len, const string& in_b64_str)
/// @brief	Base64 디코딩
/// @param	out_byte_str	[out] 디코딩 값
/// @param	out_len			[out] 디코딩 값 길이
/// @param	in_b64_str		[in] 인코딩된 입력 값
/// @return 디코딩 길이
///////////////////////////////////////////////////////////////////////////////
uint_t StrUtil::base64DecodeString(byte_t* out_byte_str, size_t& out_len, const string& in_b64_str)
{
	int in_len = (int)in_b64_str.size();
	size_t i = 0;
	size_t j = 0;
	int in_ = 0;
	unsigned char char_array_3[3] = { 0, 0, 0 };
	unsigned char char_array_4[4] = { 0, 0, 0, 0 };

	out_len = 0;
	while (in_len-- && (in_b64_str[in_] != '=') && _is_base64(in_b64_str[in_]))
	{
		char_array_4[i++] = in_b64_str[in_]; in_++;
		if (i == 4)
		{
			for (i = 0; i < 4; i++)
			{
				char_array_4[i] = static_cast<unsigned char>(_base64_chars.find(char_array_4[i]));
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; i < 3; i++)
			{
				out_byte_str[out_len++] = char_array_3[i];
			}
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 4; j++)
		{
			char_array_4[j] = 0;
		}

		for (j = 0; j < 4; j++)
		{
			char_array_4[j] = static_cast<unsigned char>(_base64_chars.find(char_array_4[j]));
		}

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++)
		{
			out_byte_str[out_len++] = char_array_3[j];
		}
	}
	
	out_byte_str[out_len] = '\0';
	return (int)out_len;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void StrUtil::StrUtil::dprintf(string* dmsg, FILE* fp, const char* fmt, ...)
/// @brief  스트링 및 File에 메시지 추가
/// @param	dmsg		[out] 스트링 변수
/// @param  fp          [in] 파일 포인터
/// @param  fmt         [in] 포맷
/// @param  ...         [in] 인수리스트
///////////////////////////////////////////////////////////////////////////////
void StrUtil::dprintf(string* dmsg, FILE* fp, const char* fmt, ...)
{
	if (dmsg == 0) return;
	if (fmt == 0) return;
	
    va_list args;
    int n, size = 1024;
    char* value = 0;     

    while (true)
    {
		if (value) 
		{			
			delete[] value;
			value = 0;
		}
		value = new char[size+1];
        if (value == 0) return;
        
        memset(value, 0, size+1);
        
        va_start(args, fmt); 
        n = vsnprintf(value, size, fmt, args);
        va_end(args);
        
        if ((n > -1) && (n < size))
        {
            value[n] = '\0';
            break;
        }
        
        size *= 2;
    }
	
	if (dmsg) (*dmsg) += value;
	if (fp)
	{
		fprintf(fp, "%s", value);
		fflush(fp);
	}
	
	if (value) 
	{
		delete[] value;
		value = 0;
	}
}

__END_NAMESPACE_JFX
