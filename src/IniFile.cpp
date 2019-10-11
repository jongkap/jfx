// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       IniFile.cpp
/// @brief      IniFile 클래스 구현
/// @author     Jongkap Jeong <kbroad@mail.com, kbroad.blog.com>
/// @date       2007/04/30 - First Release
///             2015/10/01 - 수정 작성 (Ref: https://github.com/benhoyt/inih)
/// @par
///		Ini 파일을 파싱해서 저장 후 값을 얻는 클래스\n\n
///		Ini 파일 형태\n
///		[section]\n
///		name = value\n\n
///		주석: 첫 칸에 ';', '#'인 경우, 공백 뒤에 ';' 가 있는 경우\n
///		멀티 라인: value 값이 멀티라인인 경우 '\n'값을 추가하여 지원\n
///		UTF-8 BOM 값 처리\n\n
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#include "jfx.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

__BEGIN_NAMESPACE_JFX

#if defined(_WIN32)
  #include <io.h>

  #define F_OK	0x00
  #define W_OK	0x02
  #define R_OK	0x04

  #ifndef access
    #define access _access
  #endif 
#endif


#define MAX_CONF_SECTION 	64		// 최대 섹션 버퍼 값
#define MAX_CONF_NAME		64		// 최대 네임 버퍼 값
#define MAX_CONF_LINE 		2048	// 최대 라인 버퍼 값

#ifdef __cplusplus
extern "C" {
#endif
    static char* _rstrip(char* s);
    static char* _lskip(const char* s);
    static char* _find_char_or_comment(const char* s, char c);
    static char* _strncpy0(char* dest, const char* src, size_t size);
#ifdef __cplusplus
}
#endif


IniFile::IniFile()
{
	m_lastError = -1;
}

IniFile::IniFile(const char* filename)
{
	m_lastError = loadFile(filename);
}

IniFile::~IniFile()
{
}


///////////////////////////////////////////////////////////////////////////////
// Reference: https://github.com/benhoyt/inih
///////////////////////////////////////////////////////////////////////////////

// Strip whitespace chars off end of given string, in place. Return s.
char* _rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && isspace((unsigned char)(*--p)))
        *p = '\0';
    return s;
}

// Return pointer to first non-whitespace char in given string.
char* _lskip(const char* s)
{
    while (*s && isspace((unsigned char)(*s)))
        s++;
    return (char*)s;
}

// Return pointer to first char c or ';' comment in given string, or pointer to
//   null at end of string if neither found. ';' must be prefixed by a whitespace
//   character to register as a comment.
char* _find_char_or_comment(const char* s, char c)
{
    int was_whitespace = 0;
    while (*s && *s != c && !(was_whitespace && *s == ';')) 
	{
        was_whitespace = isspace((unsigned char)(*s));
        s++;
    }
    return (char*)s;
}

// Version of strncpy that ensures dest (size bytes) is null-terminated.
char* _strncpy0(char* dest, const char* src, size_t size)
{
	strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn     int IniFile::loadFile(const char* filename)
/// @brief  파일을 읽어 메모리(맵)에 저장한다.
/// @note   INI 스타일 파일을 읽어서 메모리에 저장\n
///         [section], name = value 형태 (공백은 무시)\n
///         세미콜론 (;)은 주석으로 처리됨.
/// @param  filename    [in] IniFile 파일 위치 \n
/// @return 성공 여부 (0: 성공, -1: 파일 에러, 라인 번호: 파싱 에러)
///////////////////////////////////////////////////////////////////////////////
int IniFile::loadFile(const char* filename)
{
    char line[MAX_CONF_LINE+1]       = {0, };
    char section[MAX_CONF_SECTION+1] = {0, };
    char prev_name[MAX_CONF_NAME+1]  = {0, };

    char* start = NULL;
    char* end   = NULL;
    char* name  = NULL;
    char* value = NULL;
    int lineno  = 0;
    int error   = 0;
    
	// 파일이 존재하는지 체크
	if (filename == NULL) return -1;
	if (access (filename, F_OK) != 0) return -1;

    // 파일명 저장하기
	m_filePath = filename; 

    // 정책 파일 열기
	FILE* file = fopen(filename, "r");
    if (file == NULL) return -1;

    // 한 라인씩 읽기
    while (fgets(line, MAX_CONF_LINE, file) != NULL) 
	{
        lineno++;

        start = line;

		// UTF-8 BOM 처리 (0xEF 0xBB 0xBF)
        if (lineno == 1 && (unsigned char)start[0] == 0xEF &&
                           (unsigned char)start[1] == 0xBB &&
                           (unsigned char)start[2] == 0xBF) {
            start += 3;
        }

		start = _lskip(_rstrip(start));

        // 주석 처리
		if (*start == ';' || *start == '#') {
            // Per Python IniFileParser, allow '#' comments at start of line
        }
		// 멀티라인 처리
        else if (*prev_name && *start && start > line) {
            // Non-black line with leading whitespace, treat as continuation
            // of previous name's value (as per Python IniFileParser).
            if (!insertValue(section, prev_name, start) && !error)
                error = lineno;
        }
        else if (*start == '[') {
            // A "[section]" line
            end = _find_char_or_comment(start + 1, ']');
            if (*end == ']') {
                *end = '\0';
                _strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                // No ']' found on section line
                error = lineno;
            }
        }
        else if (*start && *start != ';') {
            // Not a comment, must be a name[=:]value pair
            end = _find_char_or_comment(start, '=');
            if (*end != '=') {
                end = _find_char_or_comment(start, ':');
            }
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = _rstrip(start);
                value = _lskip(end + 1);
                end = _find_char_or_comment(value, '\0');
                if (*end == ';')
                    *end = '\0';
                _rstrip(value);

                // Valid name[=:]value pair found, call handler
                _strncpy0(prev_name, name, sizeof(prev_name));
                if (!insertValue(section, name, value) && !error)
                    error = lineno;
            }
            else if (!error) {
                // No '=' or ':' found on name[=:]value line
                error = lineno;
            }
        }
    }

    fclose(file);
    return error;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int IniFile::insertValue(const char* section, const char* name, const char* value)
/// @brief  파일에서 읽은 내용을 메모리 맵에 추가
/// @param  section    [in] section 명
/// @param  name       [in] name 명
/// @param  value      [in] 값
/// @return 성공 여부
///////////////////////////////////////////////////////////////////////////////
int IniFile::insertValue(const char* section, const char* name, const char* value)
{
    string key = makeKey(section, name);    
    
	if (m_mapValue.count(key))
		m_mapValue[key] += "\n";
    m_mapValue[key] += value;

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn     string IniFile::makeKey(string section, string name)
/// @brief  메모리 맵에 저장할 키 생성
/// @param  section    [in] section 명
/// @param  name       [in] name 명
/// @return 키 값
///////////////////////////////////////////////////////////////////////////////
string IniFile::makeKey(string section, string name)
{
	string key = section + "=" + name;
    
	// Convert to lower case to make section/name lookups case-insensitive	
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn     string& IniFile::getFilePath()
/// @brief  설정 파일명 반환
/// @return 설정파일명
///////////////////////////////////////////////////////////////////////////////
string& IniFile::getFilePath()
{
	return m_filePath;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int IniFile::getLastError()
/// @brief  에러값 반환
/// @return 에러값
///////////////////////////////////////////////////////////////////////////////
int IniFile::getLastError()
{
	return m_lastError;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     string& IniFile::getString(const char* section, const char* name, string default_value)
/// @brief  메모리 맵에서 해당 정보를 string으로 반환한다.
/// @param  section        [in] section 값
/// @param  name           [in] name 값
/// @param  default_value  [in] 기본 값 (해당 내용이 없을 경우)
/// @return 읽은 값 (string)
///////////////////////////////////////////////////////////////////////////////
string IniFile::getString(const char* section, const char* name, string default_value)
{
    string key = makeKey(section, name);
	return m_mapValue.count(key) ? m_mapValue[key] : default_value;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int IniFile::getInteger(const char* section, const char* name, int default_value)
/// @brief  메모리 맵에서 해당 정보를 integer으로 반환한다.
/// @param  section        [in] section 값
/// @param  name           [in] name 값
/// @param  default_value  [in] 기본 값 (해당 내용이 없을 경우)
/// @return 읽은 값 (int)
///////////////////////////////////////////////////////////////////////////////
int IniFile::getInteger(const char* section, const char* name, int default_value)
{
    string valstr = getString(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     double IniFile::getReal(const char* section, const char* name, double default_value)
/// @brief  메모리 맵에서 해당 정보를 double으로 반환한다.
/// @param  section        [in] section 값
/// @param  name           [in] name 값
/// @param  default_value  [in] 기본 값 (해당 내용이 없을 경우)
/// @return 읽은 값 (double)
///////////////////////////////////////////////////////////////////////////////
double IniFile::getReal(const char* section, const char* name, double default_value)
{
    string valstr = getString(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    
    double n = strtod(value, &end);
    return end > value ? n : default_value;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     bool IniFile::getBoolean(const char* section, const char* name, bool default_value)
/// @brief  메모리 맵에서 해당 정보를 boolean으로 반환한다.
/// @param  section        [in] section 값
/// @param  name           [in] name 값
/// @param  default_value  [in] 기본 값 (해당 내용이 없을 경우)
/// @return 읽은 값 (bool)
///////////////////////////////////////////////////////////////////////////////
bool IniFile::getBoolean(const char* section, const char* name, bool default_value)
{
    string valstr = getString(section, name, "");

	// Convert to lower case to make string comparisons case-insensitive
    std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    else
        return default_value;
}

__END_NAMESPACE_JFX
