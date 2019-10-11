// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       Logger.cpp
/// @brief      Logger 클래스 구현
/// @author     Jongkap Jeong <jongkap@mail.com>
/// @date       2007/04/30 - 처음작성
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
  #include <process.h>
  #include <sys/timeb.h>
  #include <time.h>
  #include <winsock2.h>
  #include <io.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <fcntl.h>

  #ifndef snprintf
    #define snprintf _snprintf
  #endif 

#else
  #include <unistd.h>
  #include <pthread.h>
  #include <errno.h>
  #include <strings.h>
  #include <stdarg.h>
  #include <sys/time.h>
  #include <sys/syscall.h>
#endif

#include "jfx.h"

__BEGIN_NAMESPACE_JFX

#if defined(_WIN32)
  #define JFXLOCK_TYPE			CRITICAL_SECTION
  #define JFXLOCK_INIT(x)		InitializeCriticalSection((JFXLOCK_TYPE*)x)
  #define JFXLOCK_DESTROY(x)	DeleteCriticalSection((JFXLOCK_TYPE*)x)
  #define JFXLOCK_BEGIN(x)		EnterCriticalSection((JFXLOCK_TYPE*)x)
  #define JFXLOCK_END(x)		LeaveCriticalSection((JFXLOCK_TYPE*)x)
  
  #define MUTEX HANDLE
#else 
  #define JFXLOCK_TYPE			pthread_mutex_t
  #define JFXLOCK_INIT(x)		pthread_mutex_init((JFXLOCK_TYPE*)x, NULL)
  #define JFXLOCK_DESTROY(x)	pthread_mutex_destroy((JFXLOCK_TYPE*)x)
  #define JFXLOCK_BEGIN(x)		pthread_mutex_lock((JFXLOCK_TYPE*)x)
  #define JFXLOCK_END(x)		pthread_mutex_unlock((JFXLOCK_TYPE*)x)

  #define MUTEX pthread_mutex_t
#endif

LogWriter Logger;

// unnamed namespace for private objects
namespace
{
    #define LOG_BUF_MAX     (1024 * 10 * 2)
    #define ARRAY_LENGTH(x) (int)(sizeof(x)/sizeof(x[0]))
    #define ARRAY_MAX(x)    (ARRAY_LENGTH(x) - 1)
	
    const char* LevelStr[] =
    {
        "OFF",          // LOG_OFF
        "ERROR",        // LOG_ERROR
        "WARN",         // LOG_WARN
        "INFO",         // LOG_INFO
        "DEBUG",        // LOG_DEBUG
        "TRACE",        // LOG_TRACE
    };
}



LogWriter::LogWriter()
{
	m_level = 0;
	m_fp = NULL;

	m_fd = -1;

	m_lock = new JFXLOCK_TYPE;
	JFXLOCK_INIT(m_lock);
}


LogWriter::~LogWriter()
{
	final();

	if (m_lock)
	{
		JFXLOCK_DESTROY(m_lock);
		delete (JFXLOCK_TYPE*)m_lock;
		m_lock = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int LogWriter::init(const char* path, const char* level, bool flush)
/// @brief  로그파일 초기화
/// @param  path  		[in] 로그 파일 패스
/// @param  level		[in] 로그 레벨
/// @return 초기화 여부 (0: 성공, -1: 파일 에러)
///////////////////////////////////////////////////////////////////////////////
int LogWriter::init(const char* path, const char* level)
{
    // 로그 파일명이 지정되지 않으면 오류값으로 리턴
	if (path != NULL) m_path = path;
	else 
	{
		m_error.setstrerror("errno[J-1]: LogWriter::init failed, log file path is null.");
		fprintf(stdout, "JfxLogger: %s\n", getLastError());
		return -1;
	}
	
	// 로그 레벨 설정
	setLogLevel(level);

	// 기존에 파일이 오픈되었거나 버퍼가 있으면 종료함
	final();

	// 로그파일 오픈
#if defined(_WIN32)
	if (_stricmp(path, "stdout") == 0) m_fp = stdout;
#else
	if (strcasecmp(path, "stdout") == 0) m_fp = stdout;
#endif
	else m_fp = fopen(path, "a+");

	if (m_fp == NULL)
	{
		m_error.seterrno(errno);
		fprintf(stdout, "JfxLogger: Error opening file '%s': %s\n", path, getLastError());
		return -1;
	}
/*
#if defined(_WIN32)
	m_fd = _sopen(path, O_RDWR | O_CREAT | O_APPEND, _SH_DENYNO, _S_IREAD | _S_IWRITE);
#else
    m_fd = open(path, O_WRONLY | O_CREAT | O_APPEND | O_LARGEFILE, 0644);
#endif
*/
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int LogWriter::final()
/// @brief  로그파일 종료
/// @return 성공 여부 (0: 성공, -1: 에러)
///////////////////////////////////////////////////////////////////////////////
int LogWriter::final()
{
#if defined(_WIN32)
	if (m_fd >= 0) 
	{
		_close(m_fd);
		m_fd = -1;
	}
#else
	if (m_fd >= 0) 
	{
	    close(m_fd);
		m_fd = -1;
	}
#endif

	if(m_fp != NULL) {
		fflush(m_fp);
		if (m_fp != stdout) fclose(m_fp);
		m_fp = NULL;
	}

	m_fp = stdout;
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     void LogWriter::setLogLevel(const char* level)
/// @brief  로그레벨을 설정한다.
/// @param  level   [in] 로그 레벨
///////////////////////////////////////////////////////////////////////////////
void LogWriter::setLogLevel(const char* level)
{
	if (level == NULL) return;

    for (int i=0; i<ARRAY_LENGTH(LevelStr); i++)
    {
#if defined(_WIN32)
        if (_stricmp(level, LevelStr[i]) == 0)
#else
        if (strcasecmp(level, LevelStr[i]) == 0) 
#endif
        {
            m_level = (unsigned int)i;
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int LogWriter::wirteLog()
/// @brief  로그 저장
/// @return 성공 여부 (0: 성공, -1: 에러)
///////////////////////////////////////////////////////////////////////////////
int LogWriter::wirteLog()
{
	int err = 0;
	/*
	// 버퍼에 있는 로그 쓰기
	deque<string>::iterator itr;	
	for(itr = m_queue.begin(); itr != m_queue.end(); itr++)
	{
		err = fprintf(m_fp, "%s", (*itr).c_str());
		if (err < 0)
		{
			break;
		}
	}

	m_queue.clear();
	*/

	return err;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int LogWriter::push_back(const char* log_buffer)
/// @brief  로그 추가
/// @param  log_buffer	[in] 로그 버퍼
/// @return 성공 여부 (0: 성공, -1: 에러)
///////////////////////////////////////////////////////////////////////////////
int LogWriter::push_back(const char* log_buffer)
{
	int err = 0;

	JFXLOCK_BEGIN(m_lock);

#if defined(_WIN32)
//	int fd = _sopen(m_path.c_str(), O_RDWR | O_CREAT | O_APPEND, _SH_DENYNO, _S_IREAD | _S_IWRITE);
//    _write(m_fd, log_buffer, strlen(log_buffer));
//    _close(fd);
#else
//    int fd = open(m_path.c_str(), O_WRONLY | O_CREAT | O_APPEND | O_LARGEFILE, 0644);
//    write(m_fd, log_buffer, strlen(log_buffer));
//    close(fd);
#endif

//	m_mutex.lock();
//	_lock_file(m_fp);
//	_locking(fileno(m_fp), LK_LOCK, 1L);

	if(m_fp != NULL) err = fprintf(m_fp, "%s", log_buffer);
	if (err < 0) m_error.seterrno(errno);


//	_unlock_file(m_fp);
//	_locking(fileno(m_fp), LK_UNLCK, 1L);
//	m_mutex.unlock();
	JFXLOCK_END(m_lock);	
	fflush(m_fp);


	return err;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn void LogWriter::premakestr(char* prestr, int level)
/// @brief  로그 타임스탬프 생성
/// @param  prestr		[out] 타임스탬프 버퍼
/// @param  level       [in] 로그 레벨
/// @return 타임스탬프 길이
///////////////////////////////////////////////////////////////////////////////
int LogWriter::premakestr(char* prestr, int level)
{
	struct tm vtm; 
			
    // 현재 시간을 usec까지 읽는다.
    struct timeval tv;
#if defined(_WIN32)
    struct _timeb timebuf;
    _ftime_s (&timebuf);
    
    tv.tv_sec = (long)timebuf.time;
    tv.tv_usec = (long)(timebuf.millitm) * (1000000/1000);

	localtime_s(&vtm, &timebuf.time);
#else
    gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &vtm);
#endif  

    // pid, tid 설정
    unsigned long get_pid = 0;
    unsigned long get_tid = 0;
#if defined(_WIN32)
    get_pid = (unsigned long)_getpid();
    get_tid = (unsigned long)GetCurrentThreadId();
#else
    get_pid = (unsigned long)getpid();
    get_tid = (unsigned long)syscall(__NR_gettid); 
#endif

	return snprintf(prestr, LOG_BUF_MAX, "%04d-%02d-%02d %02d:%02d:%02d.%03d %-5s %lu:%lu ", 
            vtm.tm_year + 1900, vtm.tm_mon + 1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec, (int)(tv.tv_usec/1000L),
			LevelStr[level], get_pid, get_tid);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void LogWriter::log(int level, const char *fmt, ...)
/// @brief  일반 로그
/// @param  level       [in] 로그 레벨
/// @param  fmt			[in] 포맷
/// @param  ...         [in] 인수리스트
/// @return 성공 여부 (0: 성공, -1: 에러)
///////////////////////////////////////////////////////////////////////////////
int LogWriter::log(int level, const char *fmt, ...)
{
	if (!fmt) return 0;
	if (m_level <= LV_OFF) return 0;

    // 로그 레벨 체크
    if (level > ARRAY_MAX(LevelStr)) level = LV_DEBUG;
    if (m_level < level) return 0; 

    char log_buffer[LOG_BUF_MAX+1] = {0, };
	int prestrlen = 0;
	
	// 날짜 포맷
	char* star = log_buffer;
	prestrlen = premakestr(star, level);
	star += prestrlen;

	// 가변 인자 메시지 만들기
	va_list args;
	va_start(args, fmt);
	vsnprintf(star, LOG_BUF_MAX - prestrlen, fmt, args);
	va_end(args);

	// 로그 저장
	return push_back(log_buffer);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn void LogWriter::debug(int level, const char* func, const char* file,
///                        ulong_t line, const char *fmt, ...)
/// @brief  디버그 로그
/// @param  level       [in] 로그 레벨
/// @param  func        [in] 함수명
/// @param  file        [in] 파일명
/// @param  line        [in] 라인수
/// @param  fmt			[in] 포맷
/// @param  ...         [in] 인수리스트
///////////////////////////////////////////////////////////////////////////////
int LogWriter::debug(int level, const char* func, const char* file,
                        unsigned long line, const char *fmt, ...)
{
    if (!fmt) return 0;
    if (m_level < LV_DEBUG) return 0;

	// 로그 레벨 체크
    if (level > ARRAY_MAX(LevelStr)) level = LV_DEBUG;
    if (m_level < level) return 0; 

    char log_buffer[LOG_BUF_MAX+1] = {0, };

	int _size = 0;
	int prestrlen = 0;
	
	// 날짜 포맷
	char* star = log_buffer;
	prestrlen = premakestr(star, level);
	star += prestrlen;
	
	// 파일/라인/함수명 포맷
	_size = snprintf(star, LOG_BUF_MAX - prestrlen, "[%s:%lu:%s()] ",
                    file, line, func);
	prestrlen += _size;
	star += _size;
					
	// 가변 인자 메시지 만들기
	va_list args;
	va_start(args, fmt);
	vsnprintf(star, LOG_BUF_MAX - prestrlen, fmt, args);
	va_end(args);

	// 로그 저장
	return push_back(log_buffer);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int LogWriter::dump(int level, const char* func, const char* file,
///						ulong_t line, const char* prefix, unsigned char* data, size_t len)
/// @brief  덤프 로그
/// @param  level       [in] 로그 레벨
/// @param  func        [in] 함수명
/// @param  file        [in] 파일명
/// @param  line        [in] 라인수
/// @param  prefix 		[in] 프리픽스
/// @param  data   		[in] 데이터
/// @param  len         [in] 데이터 길이
///////////////////////////////////////////////////////////////////////////////
int LogWriter::dump(int level, const char* func, const char* file,
						unsigned long line, const char* prefix, const unsigned char* data, size_t len)
{
    if (m_level < LV_DEBUG) return 0;

	// 로그 레벨 체크
    if (level > ARRAY_MAX(LevelStr)) level = LV_DEBUG;
    if (m_level < level) return 0; 

	string hexdump;
	StrUtil::hexDump(&hexdump, (unsigned char*)data, len);
	
	return debug(level, func, file, line, "%s%s\n", prefix, hexdump.c_str());
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     const char* LogWriter::getLastError() const
/// @brief  에러 스트링 얻기
/// @return 에러 스트링
///////////////////////////////////////////////////////////////////////////////
const char* LogWriter::getLastError() const
{
	return m_error.strerror();
}


__END_NAMESPACE_JFX
