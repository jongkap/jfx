// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file		jfx.h
/// @brief		JFramework Header (Version 1.3)
/// @author     Jongkap Jeong <jongkap@mail.com> 
/// @date		2007/01/07 - First Release
/// 			2008/06/09 - v1.1
/// 			2015/04/01 - v1.2
/// 			2016/05/01 - v1.3
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef __JFX_H__
#define __JFX_H__

#ifndef __cplusplus
	#error JFX requires C++ compilation (use a .cpp suffix)
#endif

#if defined(_WIN32) && (VC6)
  #error This project does not support platform or program "VC6"
#endif

#if defined(_WIN32)
  #pragma warning(disable:4996)
#endif


/////////////////////////////////////////////////////////////////////////////
// Header files

#include <stdio.h>
#include <map>
#include <string>
#include <deque>
#include <sstream>
#include <fstream> 
#include <algorithm> 
using namespace std;

#ifndef __byte_t_defined
  typedef unsigned char         byte_t;                 ///< ����Ʈ �� ����
  #define __byte_t_defined
#endif
#ifndef __uint_t_defined
typedef unsigned int            uint_t;                 ///< ���� ���� �� ����
  #define __uint_t_defined
#endif
#ifndef __ulong_t_defined
typedef unsigned long           ulong_t;                ///< ���� ���� ����
  #define __ulong_t_defined
#endif
#ifndef __cstrp_t_defined
typedef const char*             cstrp_t;                ///< ���ڿ� ��� ����
  #define __cstrp_t_defined
#endif

// bit type
#ifndef __uint_8t_defined
  typedef unsigned char         uint_8t;                ///< 8bit �� ����
  #define __uint_8t_defined
#endif
#ifndef __uint_16t_defined
  typedef unsigned short int    uint_16t;               ///< 16bit �� ����
  #define __uint_16t_defined
#endif
#ifndef __uint_32t_defined
  typedef unsigned int          uint_32t;               ///< 32bit �� ����
  #define __uint_32t_defined
#endif
#if defined(_WIN32)
# ifndef __uint_64t_defined
  typedef unsigned __int64      uint_64t;               ///< 64bit �� ����
  #define __uint_64t_defined
# endif
# ifndef __ssize_t_defined
  typedef signed int            ssize_t;                ///< ssize_t �� ����
  #define __ssize_t_defined
# endif
#else
# ifndef __uint_64t_defined
  typedef unsigned long long int uint_64t;              ///< 64bit �� ����
  #define __uint_64t_defined
# endif
#endif


//---------------------------------------------------------------------------
// Endian Check
#if defined(IS_LITTLE_ENDIAN)
/* do Nothing */
#elif defined(__i386) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64)
#	define IS_LITTLE_ENDIAN 1
/* Intel Architecture */
#elif defined(_MSC_VER)
#	define IS_LITTLE_ENDIAN 1
/* All available "Windows" are Little-Endian except XBOX360. */
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__)
/* GCC style */
/* use __BYTE_ORDER__ */
#	if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#		define IS_LITTLE_ENDIAN 1
#	else
#		define IS_LITTLE_ENDIAN 0
#	endif
#elif defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__)
/* use __BIG_ENDIAN__ and __LITTLE_ENDIAN__ */
#	if defined(__LITTLE_ENDIAN__)
#		if __LITTLE_ENDIAN__
#			define IS_LITTLE_ENDIAN 1
#		else
#			define IS_LITTLE_ENDIAN 0
#		endif
#	elif defined(__BIG_ENDIAN__)
#		if __BIG_ENDIAN__
#			define IS_LITTLE_ENDIAN 0
#		else
#			define IS_LITTLE_ENDIAN 1
#		endif
#	endif
#else
/* use <endian.h> */
#	ifdef BSD
#		include <sys/endian.h>
#	else
#		include <endian.h>
#	endif
#	if __BYTE_ORDER__ == __LITTLE_ENDIAN
#		define IS_LITTLE_ENDIAN 1
#	else
#		define IS_LITTLE_ENDIAN 0
#	endif
#endif


/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

# define __BEGIN_NAMESPACE_JFX	namespace jfx {
# define __END_NAMESPACE_JFX	}


__BEGIN_NAMESPACE_JFX


//---------------------------------------------------------------------------
// header: jerror.cpp

class jerror
{
	public:
		jerror();
		virtual ~jerror();

	public:
		void seterrno(int jerrno);								///< ���� ��ȣ ����
		void setstrerror(const char* strerror);					///< ���� ��ũ�� ����
		void appendstrerror(const char* strerror);				///< ���� ��ũ�� �߰�

		const char* strerror() const;							///< ���� ��Ʈ�� ���

	protected:
		int 		m_errno;									///< ���� ��ȣ
		string		m_strerror;									///< ���� ��Ʈ��
};


//---------------------------------------------------------------------------
// header: jmutex.cpp

class jmutex
{
	public:
		jmutex();
		virtual ~jmutex();

	public:
		int lock();										///< �� ����
		int unlock();									///< �� ����

	protected:
		void*				m_mutex;
};



//---------------------------------------------------------------------------
// header: Logger.cpp

// Log level
#define LV_OFF          0   // �α� ��� ����
#define LV_ERROR        1   // ���� �α�
#define LV_WARN         2   // ��� �α�
#define LV_INFO         3   // ���� �α�
#define LV_DEBUG        4   // ����� �α�
#define LV_TRACE        5   // �� �α�

class LogWriter
{
    public: 
        LogWriter();
        virtual ~LogWriter();

    public: 
		int init(const char* path, const char* level = "info");
//				bool flush = false, int err_mode = 0);		///< �α� �ʱ�ȭ
		int final();												///< �α� ����

		void setLogLevel(const char* level);                 		///< �α� ���� ����

        int log(int level, const char *fmt, ...);           		///< �Ϲ� �α�
        int debug(int level, const char* func,
				const char* file, unsigned long line,
				const char *fmt, ...);								///< ����� �α�
        int dump(int level, const char* func,
				const char* file, unsigned long line,
				const char* prefix,
				const unsigned char* data, size_t len);				///< ���� �α�

		const char* getLastError() const;							///< ���� ��Ʈ�� ���

	private:
		int wirteLog();												///< �α� ����
		int push_back(const char* log_buffer);						///< �α� �߰�
		int premakestr(char* prestr, int level);					///< �α� Ÿ�ӽ����� ����

	private:
        string   		m_path;                                     ///< ������ �α� ���� ��ġ
        int      		m_level;									///< �α� ����
		FILE* 			m_fp;										///< ���� ������
		
		void*			m_lock;										///< ��
		jerror			m_error;									///< ����
		jmutex			m_mutex;									///< mutex
		int m_fd;
};

extern LogWriter Logger;


//---------------------------------------------------------------------------
// header: bstring.cpp

#define BSTR_SET        0x01     // ���� ó�� ��ġ ����
#define BSTR_CUR        0x02     // ���� ���� ��ġ ���� (reserved)
#define BSTR_END        0x04     // ���� ������ ��ġ ����
#define BSTR_TRIM       0x10     // Ʈ�� �ϱ�

class bstring
{
	public:
		bstring();
		bstring(byte_t* bstr, size_t size);
		bstring(const char* cstr);
		bstring(const bstring& rhs);
		virtual ~bstring();

	public:
		bool 		empty();							///< ��Ʈ�� ������� ����
		bool		end();								///< �������� ���̿� ������ ����
		size_t		offset();							///< ���� ������ ��ġ
		void		setsize(size_t n);					///< ��Ʈ�� ���� ����
		void		setaddsize(size_t n);				///< ��Ʈ�� �߰� ���� ����
		void		setoffset(size_t n);				///< ��Ʈ�� ������ ����
		void		seek(size_t n);						///< ��Ʈ�� ������ ����
		size_t 		size() const;						///< ��Ʈ�� ����
		size_t&		size();								///< ��Ʈ�� ����
		size_t 		length() const;						///< ��Ʈ�� ����
		size_t& 	length();							///< ��Ʈ�� ����
		size_t		max_size() const;					///< �ִ� ���� ũ��
		size_t&		max_size();							///< �ִ� ���� ũ��
		byte_t 		at(size_t n) const;					///< n��° ���� (zero-based)
		byte_t&		at(size_t n);						///< n��° ���� (zero-based)
		byte_t		next();								///< ���� ������ ��ȯ �� ����
		byte_t		previous();							///< ���� ������ ��ȯ �� ����
		const char*	c_str() const;						///< ��Ʈ�� const char*
		byte_t* 	data() const;						///< ��Ʈ�� byte_t*
		byte_t*& 	data();								///< ��Ʈ�� byte_t*&
		byte_t*&	dataptr();							///< ���� ������&
		size_t*		sizeptr();							///< ���� ���� ������
		void		free();								///< �Ҵ� �޸� ����
		void 		clear();							///< ���� �ʱ�ȭ
		bool		writable(size_t n);					///< ������ ���� �ִ��� ����
		void		allocation(size_t n);				///< ���� �Ҵ�
		void		alloc(size_t n);					///< ���� �Ҵ�
		void		malloc(size_t n);					///< ���� �Ҵ�
		void		malloc_and_setsize(size_t n);		///< ���� �Ҵ� �� ���� ũ�� ����
		void		reallocation(size_t n = 0);			///< ���� �� �Ҵ�
		void		realloc(size_t n = 0);				///< ���� �� �Ҵ�
		void		bstrcpy(const byte_t* bstr, size_t n);///< ��Ʈ�� ���ۿ� bstr ����
		void		bstrcpy(const char* cstr);			///< ��Ʈ�� ���ۿ� cstr ����
		void		append(const byte_t* bstr, size_t n);///< ��Ʈ�� ���ۿ� bstr �߰�
		void		append(const char* cstr);			///< ��Ʈ�� ���ۿ� cstr �߰�
		void		format(const char* fmt, ...);		///< ��Ʈ�� ���۸� ���� ��Ʈ������ ����
		void		replace(size_t pos, size_t len,
						const byte_t* bstr, size_t n);	///< ��Ʈ�� ������ pos ��ġ�� bstr�� ��ü
		void		replace(size_t pos, size_t len,
						const char* cstr);				///< ��Ʈ�� ������ pos ��ġ�� cstr�� ��ü
		size_t		read(byte_t* bstr, size_t n);		///< ��Ʈ�� ���ۿ��� n���̸�ŭ ������ �б�
		void 		push(byte_t b);						///< ��Ʈ�� ���� ���� b �߰�
		void 		pop();								///< ��Ʈ�� ���� �� ����
		void		erase(size_t n, int whence);		///< ���� ����
		
		bstring operator+ (const bstring& rhs);			///< ������(+) �����ε�
		bstring operator+ (const char* cstr);			///< ������(+) �����ε�
		friend bstring operator+ (const bstring& lhs, 
								const bstring& rhs);	///< ������(+) �����ε�
		friend bstring operator+ (const bstring& lhs, 
								const char* rhs);		///< ������(+) �����ε�
		friend bstring operator+ (const char* lhs, 
								const bstring& rhs);	///< ������(+) �����ε�
		bstring& operator+= (const bstring& rhs); 		///< ������(+=) �����ε�
		bstring& operator+= (const char* cstr); 		///< ������(+=) �����ε�
		bstring& operator= (const bstring& rhs);		///< ������(=) �����ε�
		bstring& operator= (const char* cstr);			///< ������(=) �����ε�
		byte_t operator[] (size_t n) const;				///< ������([]) �����ε�
		byte_t& operator[] (size_t n);					///< ������([]) �����ε�

	protected:
		byte_t*		m_bstr;								///< ���� ������
		size_t		m_size;								///< ���� ũ��
		size_t		m_max_size;							///< ���� �ִ� ũ��
		size_t		m_offset;							///< ���� ������
};


//---------------------------------------------------------------------------
// header: jstring.cpp

class jstring
{
	public:
		jstring();
		jstring(const jstring& cstr);
		jstring(const jstring& cstr, size_t pos, size_t len);
		jstring(const string& str);
		jstring(const string& str, size_t pos, size_t len);
		jstring(const char* s);
		jstring(const char* s, size_t n);
		jstring(size_t n, char c);
		virtual ~jstring();

	public:
		// Iterators: begin, end, rbegin, rend �� self() �Լ��� �̿�

	public:
		// Capacity
		size_t size() const 
				{ return m_str.size(); }				///< Return length of string
		size_t length() const 
				{ return m_str.length(); }				///< Return length of string
		size_t max_size() const 
				{ return m_str.max_size(); }			///< Return maximum size of string
		void resize (size_t n) 
				{ return m_str.resize(n); }				///< Resize string
		void resize (size_t n, char c) 
				{ return m_str.resize(n, c); }			///< Resize string
		size_t capacity() const 
				{ return m_str.capacity(); }			///< Return size of allocated storage
		void reserve (size_t n = 0) 
				{ m_str.reserve(n); }					///< Request a change in capacity
		void clear() 
				{ m_str.clear(); }						///< Clear string
		bool empty() const 
				{ return m_str.empty(); }				///< Test if string is empty

	public:
		// Element access
		char& operator[] (size_t pos) 
				{ return m_str[pos]; }					///< Get character of string
		const char& operator[] (size_t pos) const 
				{ return m_str[pos]; }					///< Get character of string
		char& at (size_t pos) 
				{ return m_str.at(pos); }				///< Get character in string
		const char& at (size_t pos) const 
				{ return m_str.at(pos); }				///< Get character in string

	public:
		// Modifiers
		// : ���ǵ��� ���� append, push_back, assign, insert, erase ���� self() �Լ��� �̿�
		jstring& format(const char* fmt, ...);			///< ���� ��Ʈ��
		jstring& toupper();								///< �빮�� ��ȯ
		jstring& tolower();								///< �ҹ��� ��ȯ
		jstring& trim(const char* delim = " \t\n\r\v ");///< ���� ����
		jstring& rtrim(const char* delim = " \t\n\r\v ");///< ������ ���� ����
		jstring& ltrim(const char* delim = " \t\n\r\v ");///< ���� ���� ����
		jstring& replace(const string &search, 
						const string &replace);			///< ���ڿ� ġȯ

	public:
		// String operation
		// : ���ǵ��� ���� copy, find, rcopy, substr ���� self() �Լ��� �̿�
		string& self() 
				{ return m_str; }						///< string ��ü ����
		const string& self() const 
				{ return m_str; }						///< string ��ü ����
		const char* c_str() const 
				{ return m_str.c_str(); }				///< Get C string equivalent
		const char* data() const 
				{ return m_str.data(); }				///< Get string data

		int compare(const jstring& cstr);				///< Compare string
		int compare(const jstring& cstr, size_t n);		///< Compare string
		
		int casecompare(const jstring& cstr);			///< Compare string (ignore case)
		int casecompare(const jstring& cstr, size_t n);	///< Compare string (ignore case)

		int strtoi(int base = 10);						///< Convert string to integer
		float strtof();									///< Convert string to float
		double strtod();								///< Convert string to double
		long strtol(int base = 10);						///< Convert string to long integer
		unsigned long strtoul(int base = 10);			///< Convert string to unsigned long integer


	public:
		// ������ �����ε�
		jstring& operator= (const jstring& cstr);		///< ������(=) �����ε�
		jstring& operator= (const string& str);			///< ������(=) �����ε�
		jstring& operator= (const char* s);				///< ������(=) �����ε�
		jstring& operator= (char c);					///< ������(=) �����ε�

		jstring& operator+= (const jstring& cstr);		///< ������(+=) �����ε�
		jstring& operator+= (const string& str);		///< ������(+=) �����ε�
		jstring& operator+= (const char* s);			///< ������(+=) �����ε�
		jstring& operator+= (const char c);				///< ������(+=) �����ε�

		friend jstring operator+ (jstring& lhs, 
								const jstring& rhs);	///< ������(+) �����ε�
		friend jstring operator+ (jstring& lhs, 
								const string& rhs);		///< ������(+) �����ε�
		friend jstring operator+ (jstring& lhs, 
								const char* rhs);		///< ������(+) �����ε�
		friend jstring operator+ (jstring& lhs, 
								const char rhs);		///< ������(+) �����ε�

		friend jstring operator+ (const string& lhs, 
								const jstring& rhs);	///< ������(+) �����ε�
		friend jstring operator+ (const char* lhs, 
								const jstring& rhs);	///< ������(+) �����ε�
		friend jstring operator+ (const char lhs, 
								const jstring& rhs);	///< ������(+) �����ε�

	    friend bool operator< (const jstring& lhs, 
								const jstring& rhs);	///< ������(<) �����ε�
	    friend bool operator> (const jstring& lhs, 
								const jstring& rhs);	///< ������(>) �����ε�
	    friend bool operator<= (const jstring& lhs, 
								const jstring& rhs);	///< ������(<=) �����ε�
	    friend bool operator>= (const jstring& lhs, 
								const jstring& rhs);	///< ������(>=) �����ε�

	    friend bool operator== (const jstring& lhs, 
								const jstring& rhs);	///< ������(==) �����ε�
	    friend bool operator!= (const jstring& lhs, 
								const jstring& rhs);	///< ������(!=) �����ε�

	protected:
		string		m_str;								///< ��Ʈ�� ������
};


//---------------------------------------------------------------------------
// header: IniFile.cpp
//
// usage:
//		[section]
//		name = value (�Ǵ�) name : value
//		�ּ�: ù ĭ�� ';', '#'�� ���, ���� �ڿ� ';' �� �ִ� ��� �ּ� ó��
//		��Ƽ ����: value ���� ��Ƽ������ ��� '\n'���� �߰��Ͽ� ó��
//		UTF-8 BOM �� ó��

class IniFile
{
	public:
		IniFile();
		IniFile(const char* filename);
		virtual ~IniFile();

	public:
		int loadFile(const char* filename);
		string& getFilePath();
		int getLastError();
			 
		// [section] name = value, value ���� ���º��� ���
		// �����Ͱ� ���� ��� default_value �� ����
		string getString(const char* section, const char* name, string default_value);
		int getInteger(const char* section, const char* name, int default_value);
		double getReal(const char* section, const char* name, double default_value);
		bool getBoolean(const char* section, const char* name, bool default_value);
		
	private:
		int insertValue(const char* section, const char* name, const char* value);
		string makeKey(string section, string name);

	private:
		int m_lastError;
		string m_filePath;
		map<string, string> m_mapValue;
};


//---------------------------------------------------------------------------
// header: JSocket.cpp

#include <sys/types.h>

#if defined(_WIN32)
  #include <winsock2.h>
  #include <wininet.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netinet/tcp.h>
  #include <netinet/in_systm.h>
  #include <netinet/ip.h>
  #include <unistd.h>
  #include <fcntl.h>  
#endif

#if defined(_hpux) || defined(hpux) || defined(__hpux)
  #include <sys/time.h>
#else
  #include <time.h>
#endif

#if defined(_WIN32)
  typedef int		socklen_t;
  typedef SOCKET	sockfd_t;
  #define closefd	closesocket
  #define ESOCKFD	0
#else
  typedef int		sockfd_t;
  #define closefd	close
  #define ESOCKFD	(-1)
#endif

// Receive Mode
#define R_NORMAL  		0	// �Ϲ����� �б�
#define R_LINE  		1	// CRLF���� �б�
#define R_FIXED  		2	// �Էµ� ũ�⸸ŭ �б�


/// @struct		ConnectionInfo
/// @brief		���ؼ� ������ ���� ����ü
struct ConnectionInfo
{
	sockfd_t sockfd;			///< ���� ���ϵ�ũ����
	char     ip[16];			///< ���� IP ����
	int      port;				///< ���� ��Ʈ ����
	time_t   dateTime;			///< ���� �ð�
	time_t   actionTime;		///< ������ ���� �ð� (Ÿ�ӿ��� üũ��)
};

/// @typedef	ConnectionInfo
/// @brief		���ؼ� ���� Ÿ�� ����
typedef struct ConnectionInfo ConnectionInfo;


class JSocket
{
	public:
		JSocket();
		JSocket(sockfd_t sockfd, int domain = AF_INET);
		virtual ~JSocket();

	public:
		int setSocketNonDelay();									///< Nagle �˰��� ������
		int setSocketTos(bool isInteractive);						///< IP_TOS ����
		
		void setTimeoutIsError(bool isError = true);				///< ���� Ÿ�Ӿƿ� ���� ó��
		
		int getLastError();											///< ���� ��ȣ
		const char* getLastErrorString();							///< ���� ��Ʈ��

		bool isConnected();											///< ���� ����
		sockfd_t getSocketFile();									///< ���� ��ũ���� ���
		int closeSocketFile();										///< ���� ��ũ���� �ݱ�
		char* getIpAddr();											///< ���� ���� IP ����
		int getPort();												///< ���� ���� ��Ʈ ���� ���

		bool isEof();												///< EOF ���� ���
		void setEof(bool isEof = true);								///< EOF ���� ����

		int getTimeout();											///< Ÿ�Ӿƿ� �ð� ���
		void setTimeout(int nTimeout);								///< Ÿ�Ӿƿ� �ð� ����

		bool isReceiveTimeOver(int sec = 0,
				time_t tTime = time(0));							///< Ÿ�ӿ��� üũ
		void setLastReceiveTime(time_t tTime = time(0));			///< ������ ���� �ð� ����

		ConnectionInfo* getConnectionInfo();						///< ���ؼ� ���� ���

		int nbconnect(const struct sockaddr *serv_addr,
				socklen_t addrlen, int sec = 10);					///< ���Ͽ� �����ϱ�
		int nbconnect(const char* addr, int port,
				int srcport = 0, int sec = 10);						///< �����ϱ�

		ssize_t nbsend(const void* msg, size_t len);				///< ���Ͽ� ������
		ssize_t nbsend(const bstring* bstr);						///< ���Ͽ� ������
		ssize_t nbreceive(void* buf, size_t len, 
					int mode = R_NORMAL);							///< ���Ͽ��� �ޱ�
		ssize_t nbreceive(bstring* bstr, size_t len, 
					int mode = R_NORMAL);							///< ���Ͽ��� �ޱ�
		bool nbreceive();											///< ���Ͽ��� ���۸�
		
		bstring* getBuffer();										///< ���� ���
		void getBuffer(bstring* bstr);								///< ���� ���

		bool isWriteable();											///< ���� ��������(Ÿ�Ӿƿ�) üũ
		bool isReadable();											///< �б� ��������(Ÿ�Ӿƿ�) üũ
		bool isReceiveEof();										///< EOF ��ȣ�� �ִ��� üũ

	private:
		void clearError();											///< ���� �ڵ�/�޽��� �ʱ�ȭ
		int setSocketNonBloking();									///< �ͺ�ŷ ���� ����
		int setKeepAlive(int domain = AF_INET);						///< ����(keep alive) ���� ����
		int setPeerInfo(sockfd_t sockfd, int domain);				///< ����(peer) ���� ����
		int setSockInfo(sockfd_t sockfd, const char* addr, int port); ///< ����(sock) ���� ����

		ssize_t writeFixedSize(const void* msg, size_t len);		///< ũ�⸸ŭ ������
		ssize_t readFixedSize(void* buf, size_t len);				///< ũ�⸸ŭ �б�
		ssize_t readLine(void* buf, size_t len);					///< ���� �б�

		ssize_t writeSocket(const void* msg, size_t len);			///< Ÿ�Ӿƿ� ���� ���Ͽ� ����
		ssize_t readSocket(void* buf, size_t len, int flag = 0);	///< Ÿ�Ӿƿ� ���� ���Ͽ��� �б�

	private:
		bool			m_isConnected;								///< ���� ����
		bool			m_isEof;									///< EOF ����
		int				m_nTimeout;									///< Ÿ�Ӿƿ� �ð�
		bool 			m_isTimeoutIsError;							///< ���� Ÿ�Ӿƿ��� ������ ó��
		ConnectionInfo	m_connInfo;									///< ���ؼ� ���� ����
		
		bstring			m_bstrBuffer;								///< ����
		int				m_nLastError;								///< ���� ��ȣ
		jstring			m_strLastErrorString;						///< ���� ��Ʈ��
};


//---------------------------------------------------------------------------
// header: StrUtil.cpp

// Swap 2 byte, 16 bit values:
#define JFX_BSWAP16(val) \
 ( (((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00) )

// Swap 4 byte, 32 bit values:
#  define JFX_BSWAP32(val) \
 ( (((val) >> 24) & 0x000000FF) | (((val) >>  8) & 0x0000FF00) | \
   (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )

// Swap 8 byte, 64 bit values:
#  define JFX_BSWAP64(val) \
 ( (((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
   (((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) | \
   (((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
   (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )


#if defined(IS_LITTLE_ENDIAN)
#  define jfx_htole16(val) 	(val)
#  define jfx_htole32(val) 	(val)
#  define jfx_htole64(val) 	(val)

#  define jfx_htobe16(val) 	JFX_BSWAP16 (val)
#  define jfx_htobe32(val) 	JFX_BSWAP32 (val)
#  define jfx_htobe64(val) 	JFX_BSWAP64 (val)

#  define jfx_le16toh(val) 	(val)
#  define jfx_le32toh(val) 	(val)
#  define jfx_le64toh(val) 	(val)

#  define jfx_be16toh(val) 	JFX_BSWAP16 (val)
#  define jfx_be32toh(val) 	JFX_BSWAP16 (val)
#  define jfx_be64toh(val) 	JFX_BSWAP16 (val)
#else
#  define jfx_htole16(val) 	JFX_BSWAP16 (val)
#  define jfx_htole32(val) 	JFX_BSWAP16 (val)
#  define jfx_htole64(val) 	JFX_BSWAP16 (val)

#  define jfx_htobe16(val) 	(val)
#  define jfx_htobe32(val) 	(val)
#  define jfx_htobe64(val) 	(val)

#  define jfx_le16toh(val) 	JFX_BSWAP16 (val)
#  define jfx_le32toh(val) 	JFX_BSWAP16 (val)
#  define jfx_le64toh(val) 	JFX_BSWAP16 (val)

#  define jfx_be16toh(val) 	(val)
#  define jfx_be32toh(val) 	(val)
#  define jfx_be64toh(val) 	(val)
#endif

class StrUtil
{
	public:
		static void toUpper(char *string);                    		///< �빮�� ��ȯ
		static void toLower(char *string);                    		///< �ҹ��� ��ȯ
		
		template <typename T> static string toString(T num) { 		///< ��Ʈ������ ��ȯ
			    ostringstream out;
				out << num;
				return out.str();
		}
		static const char* strError(int errnum);					///< �ý��� ���� ��Ʈ�� ��ȯ

		
        static char* strToken(register char **stringp,
                        register const char *delim);           		///< ��Ʈ�� ��ū������		
		
		static void putInt16u(void *vp, uint_16t v);            	///< 16bit �� ����
		static uint_16t getInt16u(const void *vp);              	///< 16bit �� �б�
		static void putInt32u(void *vp, uint_32t v);            	///< 32bit �� ����
		static uint_32t getInt32u(const void *vp);              	///< 32bit �� �б�
		static void putInt64u(void *vp, uint_64t v);            	///< 64bit �� ����
		static uint_64t getInt64u(const void *vp);              	///< 64bit �� �б�

        static void binToHex(string* out, const unsigned char* data,
                        size_t len);                            	///< Bin�� Hex�� ��ȯ
        static void hexToBin(unsigned char* data, size_t* len,
                        const string& in);                      	///< Hex�� Bin���� ��ȯ
		static void hexDump(string* out, const char* prefix,
						unsigned char* data, size_t len);   		///< Hex�� ���
		static void hexDump(string* out, 
						unsigned char* data, size_t len);   		///< Hex�� ���

		static uint_t base64Encode(char* out_str,
					const byte_t* in_byte_str, uint_t in_le);		///< Base64 ���ڵ�
		static uint_t base64Decode(byte_t* out_byte_str, 
						uint_t* out_len, const char* in_b64_str);	///< Base64 ���ڵ�
		static uint_t base64EncodeString(string& out_str, 
					const byte_t* in_byte_str, size_t in_len);		///< Base64 ���ڵ�
		static uint_t base64DecodeString(byte_t* out_byte_str, 
					size_t& out_len, const string& in_b64_str);		///< Base64 ���ڵ�

		static void dprintf(string* dmsg, 
					FILE* fp, const char* fmt, ...);				///< ��Ʈ�� �� File�� �޽��� �߰�
};

class SysUtil
{
	public:
		static int getOSPlatform(jstring& strPlatform);
		static int getOSUserName(jstring& strOsUserName);
		static int getHostName(jstring& strHostName);
		static int getProcessName(jstring& strProcName);
		static int getCurrentDir(jstring& strCurrentDir);
		static int getMacAddress(jstring& strMacAddr);
		static int getAllMacAddress(map<string, string>& mapMacAddr);
};

__END_NAMESPACE_JFX


/////////////////////////////////////////////////////////////////////////////
// Macro declared in this file

// macro: func (pretty function) define
#if defined(__STDC__) && (__STDC_VERSION__ < 199901L) && (__GNUC__ >= 2)
  #define __JFXFUNC__ __FUNCTION__
#elif defined(_AIX)
  #define __JFXFUNC__ ((strrchr(__func__,':') == NULL) ? __func__ : strrchr(__func__,':')+1)
#elif defined(_WIN32)
  #define __JFXFUNC__ __FUNCTION__
#else
  #define __JFXFUNC__ __func__
#endif

// macro: file (pretty function) define
#if defined(_WIN32)
  #define __JFXFILE__ ((strrchr(__FILE__,'\\') == NULL) ? __FILE__ : strrchr(__FILE__,'\\')+1)
#else
  #define __JFXFILE__ __FILE__
#endif

// macro: line define
#define __JFXLINE__ __LINE__


// �ΰ� ���
#define JfxGetLogger() \
			Logger

// �α� �ʱ�ȭ
#define JfxLogger(path, level) \
			Logger.init(path, level)

// �Ϲ� �α�
#define JfxLog(level, ...) \
			Logger.log(level,  __VA_ARGS__)

// ����� �α�
#define JfxDebug(...)	\
			Logger.debug(LV_DEBUG, __JFXFUNC__, __JFXFILE__, __JFXLINE__, __VA_ARGS__) 

#define JfxTrace(...)	\
			Logger.debug(LV_TRACE, __JFXFUNC__, __JFXFILE__, __JFXLINE__, __VA_ARGS__) 

// ���� �α�
#define JfxDump(prefix, data, len)	\
			Logger.dump(LV_TRACE, __JFXFUNC__, __JFXFILE__, __JFXLINE__, prefix, data, len) 
		
#endif // __JFX_H__
