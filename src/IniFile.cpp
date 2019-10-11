// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       IniFile.cpp
/// @brief      IniFile Ŭ���� ����
/// @author     Jongkap Jeong <kbroad@mail.com, kbroad.blog.com>
/// @date       2007/04/30 - First Release
///             2015/10/01 - ���� �ۼ� (Ref: https://github.com/benhoyt/inih)
/// @par
///		Ini ������ �Ľ��ؼ� ���� �� ���� ��� Ŭ����\n\n
///		Ini ���� ����\n
///		[section]\n
///		name = value\n\n
///		�ּ�: ù ĭ�� ';', '#'�� ���, ���� �ڿ� ';' �� �ִ� ���\n
///		��Ƽ ����: value ���� ��Ƽ������ ��� '\n'���� �߰��Ͽ� ����\n
///		UTF-8 BOM �� ó��\n\n
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


#define MAX_CONF_SECTION 	64		// �ִ� ���� ���� ��
#define MAX_CONF_NAME		64		// �ִ� ���� ���� ��
#define MAX_CONF_LINE 		2048	// �ִ� ���� ���� ��

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
/// @brief  ������ �о� �޸�(��)�� �����Ѵ�.
/// @note   INI ��Ÿ�� ������ �о �޸𸮿� ����\n
///         [section], name = value ���� (������ ����)\n
///         �����ݷ� (;)�� �ּ����� ó����.
/// @param  filename    [in] IniFile ���� ��ġ \n
/// @return ���� ���� (0: ����, -1: ���� ����, ���� ��ȣ: �Ľ� ����)
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
    
	// ������ �����ϴ��� üũ
	if (filename == NULL) return -1;
	if (access (filename, F_OK) != 0) return -1;

    // ���ϸ� �����ϱ�
	m_filePath = filename; 

    // ��å ���� ����
	FILE* file = fopen(filename, "r");
    if (file == NULL) return -1;

    // �� ���ξ� �б�
    while (fgets(line, MAX_CONF_LINE, file) != NULL) 
	{
        lineno++;

        start = line;

		// UTF-8 BOM ó�� (0xEF 0xBB 0xBF)
        if (lineno == 1 && (unsigned char)start[0] == 0xEF &&
                           (unsigned char)start[1] == 0xBB &&
                           (unsigned char)start[2] == 0xBF) {
            start += 3;
        }

		start = _lskip(_rstrip(start));

        // �ּ� ó��
		if (*start == ';' || *start == '#') {
            // Per Python IniFileParser, allow '#' comments at start of line
        }
		// ��Ƽ���� ó��
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
/// @brief  ���Ͽ��� ���� ������ �޸� �ʿ� �߰�
/// @param  section    [in] section ��
/// @param  name       [in] name ��
/// @param  value      [in] ��
/// @return ���� ����
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
/// @brief  �޸� �ʿ� ������ Ű ����
/// @param  section    [in] section ��
/// @param  name       [in] name ��
/// @return Ű ��
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
/// @brief  ���� ���ϸ� ��ȯ
/// @return �������ϸ�
///////////////////////////////////////////////////////////////////////////////
string& IniFile::getFilePath()
{
	return m_filePath;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int IniFile::getLastError()
/// @brief  ������ ��ȯ
/// @return ������
///////////////////////////////////////////////////////////////////////////////
int IniFile::getLastError()
{
	return m_lastError;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     string& IniFile::getString(const char* section, const char* name, string default_value)
/// @brief  �޸� �ʿ��� �ش� ������ string���� ��ȯ�Ѵ�.
/// @param  section        [in] section ��
/// @param  name           [in] name ��
/// @param  default_value  [in] �⺻ �� (�ش� ������ ���� ���)
/// @return ���� �� (string)
///////////////////////////////////////////////////////////////////////////////
string IniFile::getString(const char* section, const char* name, string default_value)
{
    string key = makeKey(section, name);
	return m_mapValue.count(key) ? m_mapValue[key] : default_value;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int IniFile::getInteger(const char* section, const char* name, int default_value)
/// @brief  �޸� �ʿ��� �ش� ������ integer���� ��ȯ�Ѵ�.
/// @param  section        [in] section ��
/// @param  name           [in] name ��
/// @param  default_value  [in] �⺻ �� (�ش� ������ ���� ���)
/// @return ���� �� (int)
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
/// @brief  �޸� �ʿ��� �ش� ������ double���� ��ȯ�Ѵ�.
/// @param  section        [in] section ��
/// @param  name           [in] name ��
/// @param  default_value  [in] �⺻ �� (�ش� ������ ���� ���)
/// @return ���� �� (double)
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
/// @brief  �޸� �ʿ��� �ش� ������ boolean���� ��ȯ�Ѵ�.
/// @param  section        [in] section ��
/// @param  name           [in] name ��
/// @param  default_value  [in] �⺻ �� (�ش� ������ ���� ���)
/// @return ���� �� (bool)
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
