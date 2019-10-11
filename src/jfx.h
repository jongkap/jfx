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
  typedef unsigned char         byte_t;                 ///< 바이트 형 정의
  #define __byte_t_defined
#endif
#ifndef __uint_t_defined
typedef unsigned int            uint_t;                 ///< 양의 정수 형 정의
  #define __uint_t_defined
#endif
#ifndef __ulong_t_defined
typedef unsigned long           ulong_t;                ///< 양의 롱형 정의
  #define __ulong_t_defined
#endif
#ifndef __cstrp_t_defined
typedef const char*             cstrp_t;                ///< 문자열 상수 정의
  #define __cstrp_t_defined
#endif

// bit type
#ifndef __uint_8t_defined
  typedef unsigned char         uint_8t;                ///< 8bit 형 정의
  #define __uint_8t_defined
#endif
#ifndef __uint_16t_defined
  typedef unsigned short int    uint_16t;               ///< 16bit 형 정의
  #define __uint_16t_defined
#endif
#ifndef __uint_32t_defined
  typedef unsigned int          uint_32t;               ///< 32bit 형 정의
  #define __uint_32t_defined
#endif
#if defined(_WIN32)
# ifndef __uint_64t_defined
  typedef unsigned __int64      uint_64t;               ///< 64bit 형 정의
  #define __uint_64t_defined
# endif
# ifndef __ssize_t_defined
  typedef signed int            ssize_t;                ///< ssize_t 형 정의
  #define __ssize_t_defined
# endif
#else
# ifndef __uint_64t_defined
  typedef unsigned long long int uint_64t;              ///< 64bit 형 정의
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
		void seterrno(int jerrno);								///< 에러 번호 설정
		void setstrerror(const char* strerror);					///< 에러 스크링 설정
		void appendstrerror(const char* strerror);				///< 에러 스크링 추가

		const char* strerror() const;							///< 에러 스트링 얻기

	protected:
		int 		m_errno;									///< 에러 번호
		string		m_strerror;									///< 에러 스트링
};


//---------------------------------------------------------------------------
// header: jmutex.cpp

class jmutex
{
	public:
		jmutex();
		virtual ~jmutex();

	public:
		int lock();										///< 락 설정
		int unlock();									///< 락 해제

	protected:
		void*				m_mutex;
};



//---------------------------------------------------------------------------
// header: Logger.cpp

// Log level
#define LV_OFF          0   // 로그 사용 안함
#define LV_ERROR        1   // 에러 로그
#define LV_WARN         2   // 경고 로그
#define LV_INFO         3   // 정보 로그
#define LV_DEBUG        4   // 디버깅 로그
#define LV_TRACE        5   // 상세 로그

class LogWriter
{
    public: 
        LogWriter();
        virtual ~LogWriter();

    public: 
		int init(const char* path, const char* level = "info");
//				bool flush = false, int err_mode = 0);		///< 로그 초기화
		int final();												///< 로그 종료

		void setLogLevel(const char* level);                 		///< 로그 레벨 설정

        int log(int level, const char *fmt, ...);           		///< 일반 로그
        int debug(int level, const char* func,
				const char* file, unsigned long line,
				const char *fmt, ...);								///< 디버그 로그
        int dump(int level, const char* func,
				const char* file, unsigned long line,
				const char* prefix,
				const unsigned char* data, size_t len);				///< 덤프 로그

		const char* getLastError() const;							///< 에러 스트링 얻기

	private:
		int wirteLog();												///< 로그 저장
		int push_back(const char* log_buffer);						///< 로그 추가
		int premakestr(char* prestr, int level);					///< 로그 타임스탬프 생성

	private:
        string   		m_path;                                     ///< 설정된 로그 파일 위치
        int      		m_level;									///< 로그 레벨
		FILE* 			m_fp;										///< 파일 포인터
		
		void*			m_lock;										///< 락
		jerror			m_error;									///< 에러
		jmutex			m_mutex;									///< mutex
		int m_fd;
};

extern LogWriter Logger;


//---------------------------------------------------------------------------
// header: bstring.cpp

#define BSTR_SET        0x01     // 버퍼 처음 위치 기준
#define BSTR_CUR        0x02     // 버퍼 현재 위치 기준 (reserved)
#define BSTR_END        0x04     // 버퍼 마지막 위치 기준
#define BSTR_TRIM       0x10     // 트림 하기

class bstring
{
	public:
		bstring();
		bstring(byte_t* bstr, size_t size);
		bstring(const char* cstr);
		bstring(const bstring& rhs);
		virtual ~bstring();

	public:
		bool 		empty();							///< 스트링 비어있음 여부
		bool		end();								///< 오프셋이 길이와 같은지 여부
		size_t		offset();							///< 현재 오프셋 위치
		void		setsize(size_t n);					///< 스트링 길이 설정
		void		setaddsize(size_t n);				///< 스트링 추가 길이 설정
		void		setoffset(size_t n);				///< 스트링 오프셋 설정
		void		seek(size_t n);						///< 스트링 오프셋 설정
		size_t 		size() const;						///< 스트링 길이
		size_t&		size();								///< 스트링 길이
		size_t 		length() const;						///< 스트링 길이
		size_t& 	length();							///< 스트링 길이
		size_t		max_size() const;					///< 최대 버퍼 크기
		size_t&		max_size();							///< 최대 버퍼 크기
		byte_t 		at(size_t n) const;					///< n번째 글자 (zero-based)
		byte_t&		at(size_t n);						///< n번째 글자 (zero-based)
		byte_t		next();								///< 현재 오프셋 반환 및 증가
		byte_t		previous();							///< 현재 오프셋 반환 및 감소
		const char*	c_str() const;						///< 스트링 const char*
		byte_t* 	data() const;						///< 스트링 byte_t*
		byte_t*& 	data();								///< 스트링 byte_t*&
		byte_t*&	dataptr();							///< 버퍼 포인터&
		size_t*		sizeptr();							///< 버퍼 길이 포인터
		void		free();								///< 할당 메모리 해제
		void 		clear();							///< 버퍼 초기화
		bool		writable(size_t n);					///< 저장할 공간 있는지 여부
		void		allocation(size_t n);				///< 버퍼 할당
		void		alloc(size_t n);					///< 버퍼 할당
		void		malloc(size_t n);					///< 버퍼 할당
		void		malloc_and_setsize(size_t n);		///< 버퍼 할당 및 버퍼 크기 설정
		void		reallocation(size_t n = 0);			///< 버퍼 재 할당
		void		realloc(size_t n = 0);				///< 버퍼 재 할당
		void		bstrcpy(const byte_t* bstr, size_t n);///< 스트링 버퍼에 bstr 저장
		void		bstrcpy(const char* cstr);			///< 스트링 버퍼에 cstr 저장
		void		append(const byte_t* bstr, size_t n);///< 스트링 버퍼에 bstr 추가
		void		append(const char* cstr);			///< 스트링 버퍼에 cstr 추가
		void		format(const char* fmt, ...);		///< 스트링 버퍼를 포맷 스트링으로 설정
		void		replace(size_t pos, size_t len,
						const byte_t* bstr, size_t n);	///< 스트링 버퍼의 pos 위치에 bstr로 교체
		void		replace(size_t pos, size_t len,
						const char* cstr);				///< 스트링 버퍼의 pos 위치에 cstr로 교체
		size_t		read(byte_t* bstr, size_t n);		///< 스트링 버퍼에서 n길이만큼 데이터 읽기
		void 		push(byte_t b);						///< 스트링 버퍼 끝에 b 추가
		void 		pop();								///< 스트링 버퍼 끝 제거
		void		erase(size_t n, int whence);		///< 버퍼 삭제
		
		bstring operator+ (const bstring& rhs);			///< 연산자(+) 오버로딩
		bstring operator+ (const char* cstr);			///< 연산자(+) 오버로딩
		friend bstring operator+ (const bstring& lhs, 
								const bstring& rhs);	///< 연산자(+) 오버로딩
		friend bstring operator+ (const bstring& lhs, 
								const char* rhs);		///< 연산자(+) 오버로딩
		friend bstring operator+ (const char* lhs, 
								const bstring& rhs);	///< 연산자(+) 오버로딩
		bstring& operator+= (const bstring& rhs); 		///< 연산자(+=) 오버로딩
		bstring& operator+= (const char* cstr); 		///< 연산자(+=) 오버로딩
		bstring& operator= (const bstring& rhs);		///< 연산자(=) 오버로딩
		bstring& operator= (const char* cstr);			///< 연산자(=) 오버로딩
		byte_t operator[] (size_t n) const;				///< 연산자([]) 오버로딩
		byte_t& operator[] (size_t n);					///< 연산자([]) 오버로딩

	protected:
		byte_t*		m_bstr;								///< 버퍼 데이터
		size_t		m_size;								///< 버퍼 크기
		size_t		m_max_size;							///< 버퍼 최대 크기
		size_t		m_offset;							///< 버퍼 오프셋
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
		// Iterators: begin, end, rbegin, rend 는 self() 함수를 이용

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
		// : 정의되지 않은 append, push_back, assign, insert, erase 등은 self() 함수를 이용
		jstring& format(const char* fmt, ...);			///< 포맷 스트링
		jstring& toupper();								///< 대문자 변환
		jstring& tolower();								///< 소문자 변환
		jstring& trim(const char* delim = " \t\n\r\v ");///< 공백 제거
		jstring& rtrim(const char* delim = " \t\n\r\v ");///< 오른쪽 공백 제거
		jstring& ltrim(const char* delim = " \t\n\r\v ");///< 왼쪽 공백 제거
		jstring& replace(const string &search, 
						const string &replace);			///< 문자열 치환

	public:
		// String operation
		// : 정의되지 않은 copy, find, rcopy, substr 등은 self() 함수를 이용
		string& self() 
				{ return m_str; }						///< string 자체 리턴
		const string& self() const 
				{ return m_str; }						///< string 자체 리턴
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
		// 연산자 오버로딩
		jstring& operator= (const jstring& cstr);		///< 연산자(=) 오버로딩
		jstring& operator= (const string& str);			///< 연산자(=) 오버로딩
		jstring& operator= (const char* s);				///< 연산자(=) 오버로딩
		jstring& operator= (char c);					///< 연산자(=) 오버로딩

		jstring& operator+= (const jstring& cstr);		///< 연산자(+=) 오버로딩
		jstring& operator+= (const string& str);		///< 연산자(+=) 오버로딩
		jstring& operator+= (const char* s);			///< 연산자(+=) 오버로딩
		jstring& operator+= (const char c);				///< 연산자(+=) 오버로딩

		friend jstring operator+ (jstring& lhs, 
								const jstring& rhs);	///< 연산자(+) 오버로딩
		friend jstring operator+ (jstring& lhs, 
								const string& rhs);		///< 연산자(+) 오버로딩
		friend jstring operator+ (jstring& lhs, 
								const char* rhs);		///< 연산자(+) 오버로딩
		friend jstring operator+ (jstring& lhs, 
								const char rhs);		///< 연산자(+) 오버로딩

		friend jstring operator+ (const string& lhs, 
								const jstring& rhs);	///< 연산자(+) 오버로딩
		friend jstring operator+ (const char* lhs, 
								const jstring& rhs);	///< 연산자(+) 오버로딩
		friend jstring operator+ (const char lhs, 
								const jstring& rhs);	///< 연산자(+) 오버로딩

	    friend bool operator< (const jstring& lhs, 
								const jstring& rhs);	///< 연산자(<) 오버로딩
	    friend bool operator> (const jstring& lhs, 
								const jstring& rhs);	///< 연산자(>) 오버로딩
	    friend bool operator<= (const jstring& lhs, 
								const jstring& rhs);	///< 연산자(<=) 오버로딩
	    friend bool operator>= (const jstring& lhs, 
								const jstring& rhs);	///< 연산자(>=) 오버로딩

	    friend bool operator== (const jstring& lhs, 
								const jstring& rhs);	///< 연산자(==) 오버로딩
	    friend bool operator!= (const jstring& lhs, 
								const jstring& rhs);	///< 연산자(!=) 오버로딩

	protected:
		string		m_str;								///< 스트링 데이터
};


//---------------------------------------------------------------------------
// header: IniFile.cpp
//
// usage:
//		[section]
//		name = value (또는) name : value
//		주석: 첫 칸에 ';', '#'인 경우, 공백 뒤에 ';' 가 있는 경우 주석 처리
//		멀티 라인: value 값이 멀티라인인 경우 '\n'값을 추가하여 처리
//		UTF-8 BOM 값 처리

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
			 
		// [section] name = value, value 값을 행태별로 얻기
		// 데이터가 없을 경우 default_value 값 리턴
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
#define R_NORMAL  		0	// 일반적인 읽기
#define R_LINE  		1	// CRLF까지 읽기
#define R_FIXED  		2	// 입력된 크기만큼 읽기


/// @struct		ConnectionInfo
/// @brief		컨넥션 정보를 담을 구조체
struct ConnectionInfo
{
	sockfd_t sockfd;			///< 소켓 파일디스크립터
	char     ip[16];			///< 접속 IP 정보
	int      port;				///< 접속 포트 정보
	time_t   dateTime;			///< 접속 시간
	time_t   actionTime;		///< 마지막 응답 시간 (타임오버 체크용)
};

/// @typedef	ConnectionInfo
/// @brief		컨넥션 정보 타입 정의
typedef struct ConnectionInfo ConnectionInfo;


class JSocket
{
	public:
		JSocket();
		JSocket(sockfd_t sockfd, int domain = AF_INET);
		virtual ~JSocket();

	public:
		int setSocketNonDelay();									///< Nagle 알고리즘 사용안함
		int setSocketTos(bool isInteractive);						///< IP_TOS 설정
		
		void setTimeoutIsError(bool isError = true);				///< 소켓 타임아웃 에러 처리
		
		int getLastError();											///< 에러 번호
		const char* getLastErrorString();							///< 에러 스트링

		bool isConnected();											///< 접속 여부
		sockfd_t getSocketFile();									///< 소켓 디스크립터 얻기
		int closeSocketFile();										///< 소켓 디스크립터 닫기
		char* getIpAddr();											///< 소켓 접속 IP 정보
		int getPort();												///< 소켓 접속 포트 정보 얻기

		bool isEof();												///< EOF 여부 얻기
		void setEof(bool isEof = true);								///< EOF 여부 설정

		int getTimeout();											///< 타임아웃 시간 얻기
		void setTimeout(int nTimeout);								///< 타임아웃 시간 설정

		bool isReceiveTimeOver(int sec = 0,
				time_t tTime = time(0));							///< 타임오버 체크
		void setLastReceiveTime(time_t tTime = time(0));			///< 마지막 응답 시간 설정

		ConnectionInfo* getConnectionInfo();						///< 컨넥션 정보 얻기

		int nbconnect(const struct sockaddr *serv_addr,
				socklen_t addrlen, int sec = 10);					///< 소켓에 연결하기
		int nbconnect(const char* addr, int port,
				int srcport = 0, int sec = 10);						///< 접속하기

		ssize_t nbsend(const void* msg, size_t len);				///< 소켓에 보내기
		ssize_t nbsend(const bstring* bstr);						///< 소켓에 보내기
		ssize_t nbreceive(void* buf, size_t len, 
					int mode = R_NORMAL);							///< 소켓에서 받기
		ssize_t nbreceive(bstring* bstr, size_t len, 
					int mode = R_NORMAL);							///< 소켓에서 받기
		bool nbreceive();											///< 소켓에서 버퍼링
		
		bstring* getBuffer();										///< 버퍼 얻기
		void getBuffer(bstring* bstr);								///< 버퍼 얻기

		bool isWriteable();											///< 쓰기 가능한지(타임아웃) 체크
		bool isReadable();											///< 읽기 가능한지(타임아웃) 체크
		bool isReceiveEof();										///< EOF 신호가 있는지 체크

	private:
		void clearError();											///< 에러 코드/메시지 초기화
		int setSocketNonBloking();									///< 넌블러킹 소켓 설정
		int setKeepAlive(int domain = AF_INET);						///< 소켓(keep alive) 정보 설정
		int setPeerInfo(sockfd_t sockfd, int domain);				///< 소켓(peer) 정보 설정
		int setSockInfo(sockfd_t sockfd, const char* addr, int port); ///< 소켓(sock) 정보 설정

		ssize_t writeFixedSize(const void* msg, size_t len);		///< 크기만큼 보내기
		ssize_t readFixedSize(void* buf, size_t len);				///< 크기만큼 읽기
		ssize_t readLine(void* buf, size_t len);					///< 한줄 읽기

		ssize_t writeSocket(const void* msg, size_t len);			///< 타임아웃 지원 소켓에 쓰기
		ssize_t readSocket(void* buf, size_t len, int flag = 0);	///< 타임아웃 지원 소켓에서 읽기

	private:
		bool			m_isConnected;								///< 접속 여부
		bool			m_isEof;									///< EOF 여부
		int				m_nTimeout;									///< 타임아웃 시간
		bool 			m_isTimeoutIsError;							///< 소켓 타임아웃을 에러로 처리
		ConnectionInfo	m_connInfo;									///< 컨넥션 정보 변수
		
		bstring			m_bstrBuffer;								///< 버퍼
		int				m_nLastError;								///< 에러 번호
		jstring			m_strLastErrorString;						///< 에러 스트링
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
		static void toUpper(char *string);                    		///< 대문자 변환
		static void toLower(char *string);                    		///< 소문자 변환
		
		template <typename T> static string toString(T num) { 		///< 스트링으로 변환
			    ostringstream out;
				out << num;
				return out.str();
		}
		static const char* strError(int errnum);					///< 시스템 에러 스트링 반환

		
        static char* strToken(register char **stringp,
                        register const char *delim);           		///< 스트링 토큰나이저		
		
		static void putInt16u(void *vp, uint_16t v);            	///< 16bit 값 설정
		static uint_16t getInt16u(const void *vp);              	///< 16bit 값 읽기
		static void putInt32u(void *vp, uint_32t v);            	///< 32bit 값 설정
		static uint_32t getInt32u(const void *vp);              	///< 32bit 값 읽기
		static void putInt64u(void *vp, uint_64t v);            	///< 64bit 값 설정
		static uint_64t getInt64u(const void *vp);              	///< 64bit 값 읽기

        static void binToHex(string* out, const unsigned char* data,
                        size_t len);                            	///< Bin을 Hex로 변환
        static void hexToBin(unsigned char* data, size_t* len,
                        const string& in);                      	///< Hex를 Bin으로 변환
		static void hexDump(string* out, const char* prefix,
						unsigned char* data, size_t len);   		///< Hex로 출력
		static void hexDump(string* out, 
						unsigned char* data, size_t len);   		///< Hex로 출력

		static uint_t base64Encode(char* out_str,
					const byte_t* in_byte_str, uint_t in_le);		///< Base64 인코딩
		static uint_t base64Decode(byte_t* out_byte_str, 
						uint_t* out_len, const char* in_b64_str);	///< Base64 디코딩
		static uint_t base64EncodeString(string& out_str, 
					const byte_t* in_byte_str, size_t in_len);		///< Base64 인코딩
		static uint_t base64DecodeString(byte_t* out_byte_str, 
					size_t& out_len, const string& in_b64_str);		///< Base64 디코딩

		static void dprintf(string* dmsg, 
					FILE* fp, const char* fmt, ...);				///< 스트링 및 File에 메시지 추가
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


// 로거 얻기
#define JfxGetLogger() \
			Logger

// 로그 초기화
#define JfxLogger(path, level) \
			Logger.init(path, level)

// 일반 로그
#define JfxLog(level, ...) \
			Logger.log(level,  __VA_ARGS__)

// 디버그 로그
#define JfxDebug(...)	\
			Logger.debug(LV_DEBUG, __JFXFUNC__, __JFXFILE__, __JFXLINE__, __VA_ARGS__) 

#define JfxTrace(...)	\
			Logger.debug(LV_TRACE, __JFXFUNC__, __JFXFILE__, __JFXLINE__, __VA_ARGS__) 

// 덤프 로그
#define JfxDump(prefix, data, len)	\
			Logger.dump(LV_TRACE, __JFXFUNC__, __JFXFILE__, __JFXLINE__, prefix, data, len) 
		
#endif // __JFX_H__
