// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       JSocket.cpp
/// @brief      JSocket 클래스 구현
/// @author     Jongkap Jeong <jongkap@mail.com>
/// @date       2008/06/09 - 처음작성
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <errno.h>
#include "jfx.h"

#if defined(_WIN32)
  #ifndef WSAEAGAIN 
    #define WSAEAGAIN WSAEWOULDBLOCK 
  #endif 
  #pragma comment(lib, "Ws2_32.lib")
#else
  #include <unistd.h>
  #include <fcntl.h>
#endif

__BEGIN_NAMESPACE_JFX

#define IO_BUF_MAX 1024  ///< 확장할 IO 버퍼 크기


#ifdef __cplusplus
extern "C" {
#endif
    static int _geterrno();
#ifdef __cplusplus
}
#endif


JSocket::JSocket()
{
    m_isConnected = false;
    m_isEof = false;
    m_nTimeout = 60;
	m_nLastError = 0;
    memset(&m_connInfo, 0, sizeof(m_connInfo));
    
    m_isTimeoutIsError = true;
    m_connInfo.sockfd = ESOCKFD;

#if defined(_WIN32)
    WSADATA wsa;
	int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (err != 0) 
    {
		m_nLastError = err;
		m_strLastErrorString = "JSocket@could not find a usable winsock dll";
        m_isEof = true;
        return;
    }
#endif
}

JSocket::JSocket(sockfd_t sockfd, int domain)
{
    m_isConnected = true;
    m_isEof = false;
    m_nTimeout = 60;
	m_nLastError = 0;
    memset(&m_connInfo, 0, sizeof(m_connInfo));
    
    m_isTimeoutIsError = true;
    m_connInfo.sockfd = ESOCKFD;

    // 소켓(peer) 정보 설정
    setPeerInfo(sockfd, domain);
    
    // 소켓(keep alive) 정보 설정
    setKeepAlive(domain);

    // 넌블러킹 소켓 설정
    setSocketNonBloking();
}

JSocket::~JSocket()
{
    if (getSocketFile() > ESOCKFD)
    {
        closefd(m_connInfo.sockfd);
        m_connInfo.sockfd = ESOCKFD;
#if defined(_WIN32)
        WSACleanup();
#endif
    }
}


int _geterrno()
{
#if defined(_WIN32)
	return WSAGetLastError();
#else
	return errno;
#endif
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::setTimeoutIsError(bool isError)
/// @brief  소켓 타임아웃 에러 처리
/// @param  isError [in] 에러 처리 여부 여부 (기본값: true)
///////////////////////////////////////////////////////////////////////////////
void JSocket::setTimeoutIsError(bool isError)
{
    m_isTimeoutIsError = isError;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isConnected()
/// @brief  접속 여부 얻기
/// @return 접속 여부
///////////////////////////////////////////////////////////////////////////////
bool JSocket::isConnected()
{
    return m_isConnected;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn sockfd_t JSocket::getSocketFile()
/// @brief  컨넥션의 소켓 디스크립터 얻기
/// @return 소켓 파일 디스크립터
///////////////////////////////////////////////////////////////////////////////
sockfd_t JSocket::getSocketFile()
{
    return m_connInfo.sockfd;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::closeSocketFile()
/// @brief  컨넥션의 소켓 디스크립터 닫기
/// @return 소켓 파일 디스크립터
///////////////////////////////////////////////////////////////////////////////
int JSocket::closeSocketFile()
{
	if (m_connInfo.sockfd > ESOCKFD)
	{
        closefd(m_connInfo.sockfd);
        m_connInfo.sockfd = ESOCKFD;

		m_isConnected = false;
		m_isEof = false;
		m_nLastError = 0;
		m_strLastErrorString.clear();
		memset(&m_connInfo, 0, sizeof(m_connInfo));
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn char* JSocket::getIpAddr()
/// @brief  소켓 접속 IP 정보
/// @return 소켓 접속 IP
///////////////////////////////////////////////////////////////////////////////
char* JSocket::getIpAddr()
{
    return m_connInfo.ip;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn char* JSocket::getPort()
/// @brief  소켓 접속 포트 정보 얻기
/// @return 소켓 접속 포트
///////////////////////////////////////////////////////////////////////////////
int JSocket::getPort()
{
    return m_connInfo.port;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isEof()
/// @brief  EOF 여부 얻기
/// @return EOF 여부
///////////////////////////////////////////////////////////////////////////////
bool JSocket::isEof()
{
    return m_isEof;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::setEof(bool isEof)
/// @brief  EOF 여부 설정
/// @param  isEof       [in] EOF 여부 (기본값: true)
/// @return EOF 설정
///////////////////////////////////////////////////////////////////////////////
void JSocket::setEof(bool isEof)
{
    m_isEof = isEof;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::getTimeout()
/// @brief  타임아웃 시간 얻기
/// @return EOF 여부
///////////////////////////////////////////////////////////////////////////////
int JSocket::getTimeout()
{
    return m_nTimeout;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::setTimeout(int nTimeout)
/// @brief  타임아웃 시간 설정
/// @param  nTimeout    [in] 타임아웃 시간
/// @brief  EOF 설정
///////////////////////////////////////////////////////////////////////////////
void JSocket::setTimeout(int nTimeout)
{
    m_nTimeout = nTimeout;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isReceiveTimeOver(int sec, time_t tTime)
/// @brief  타임오버 체크
/// @param  sec     [in] 타임아웃 초
/// @param  tTime   [in] 시간
/// @return 타임오버 여부
///////////////////////////////////////////////////////////////////////////////
bool JSocket::isReceiveTimeOver(int sec, time_t tTime)
{
    double elapsed_time;
    int nTimeout = sec;
    if (sec == 0) nTimeout = getTimeout();

    elapsed_time = difftime(tTime, m_connInfo.actionTime);

    if (elapsed_time >= nTimeout) return true;
    return false;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::setLastReceiveTime(time_t tTime)
/// @brief  마지막 응답 시간 설정
/// @param  tTime   [in] 시간
///////////////////////////////////////////////////////////////////////////////
void JSocket::setLastReceiveTime(time_t tTime)
{
    m_connInfo.actionTime = tTime;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ConnectionInfo* JSocket::getConnectionInfo()
/// @brief  컨넥션 정보 얻기
/// @return 컨넥션 정보
///////////////////////////////////////////////////////////////////////////////
ConnectionInfo* JSocket::getConnectionInfo()
{
    return &m_connInfo;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::getLastError()
/// @brief  에러 번호 얻기
/// @return 에러 번호
///////////////////////////////////////////////////////////////////////////////
int JSocket::getLastError()
{
    return m_nLastError;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn const char* JSocket::getLastErrorString()
/// @brief  에러 스트링 얻기
/// @return 에러 스트링
///////////////////////////////////////////////////////////////////////////////
const char* JSocket::getLastErrorString()
{
    return m_strLastErrorString.c_str();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::clearError()
/// @brief  에러 코드/메시지 초기화
///////////////////////////////////////////////////////////////////////////////
void JSocket::clearError()
{
	m_nLastError = 0;
	m_strLastErrorString = "";
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setKeepAlive(int domain)
/// @brief  소켓(keep alive) 정보 설정
/// @param  domain  [in] 소켓 도메인
/// @return 성공여부 (성공: 0, 실패: -1)
///////////////////////////////////////////////////////////////////////////////
int JSocket::setKeepAlive(int domain)
{
	int on = 1;
	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return -1;
	}
    
    // socket 정보 얻기
    if (domain == AF_INET)
    {
        // socket option 설정
        if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on)) != 0)
        {
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "setKeepAlive/setsockopt/SO_KEEPALIVE", StrUtil::strError(m_nLastError));
            m_isEof = true;
            return -1;
        }
    }
    
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setPeerInfo(sockfd_t sockfd, int domain)
/// @brief  소켓(peer) 정보 얻기
/// @param  sockfd  [in] 소켓 파일 디스크립터
/// @param  domain  [in] 소켓 도메인
/// @return 성공여부
///////////////////////////////////////////////////////////////////////////////
int JSocket::setPeerInfo(sockfd_t sockfd, int domain)
{
    struct sockaddr_in so_addr;
#if defined(_hpux) || defined(hpux) || defined(__hpux)
    int addr_len;
#elif defined(_SCO_DS) || defined(__USLC__)
    size_t addr_len;
#else
    socklen_t addr_len;
#endif
    addr_len = sizeof(so_addr);

	clearError();
    
    // socket 정보 얻기
    if (domain == AF_INET)
    {
        if (getpeername(sockfd, (struct sockaddr*)&so_addr, &addr_len) != 0)
        {
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "setPeerInfo/getpeername", StrUtil::strError(m_nLastError));
            m_isEof = true;
        }
    }

    // 컨넥션 정보 설정하기
    m_connInfo.sockfd       = sockfd;
    if (domain == AF_INET)  strcpy(m_connInfo.ip, inet_ntoa(so_addr.sin_addr));
    m_connInfo.port         = ntohs(so_addr.sin_port);
    m_connInfo.dateTime     = time(0);
    m_connInfo.actionTime   = m_connInfo.dateTime;

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setSockInfo(sockfd_t sockfd, const char* addr, int port)
/// @brief  소켓(sock) 정보 얻기
/// @param  sockfd  [in] 소켓 파일 디스크립터
/// @param  addr    [in] 주소
/// @param  port    [in] 포트
/// @return 성공여부
///////////////////////////////////////////////////////////////////////////////
int JSocket::setSockInfo(sockfd_t sockfd, const char* addr, int port)
{
    // 컨넥션 정보 설정하기
    m_connInfo.sockfd       = sockfd;
    strcpy(m_connInfo.ip,   addr);
    m_connInfo.port         = port;
    m_connInfo.dateTime     = time(0);
    m_connInfo.actionTime   = m_connInfo.dateTime;

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setSocketNonBloking()
/// @brief  넌블러킹 소켓 설정
/// @return 성공여부
///////////////////////////////////////////////////////////////////////////////
int JSocket::setSocketNonBloking()
{
	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return -1;
	}

    int flags;

#if defined(_WIN32)
	flags = 1;
    if ((flags = ioctlsocket(sockfd, FIONBIO, (ULONG*)&flags)) == SOCKET_ERROR)   
    {
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "setSocketNonBloking/ioctlsocket/FIONBIO", StrUtil::strError(m_nLastError));
        return -1;
    }
#else
    // 파일디스크립터 플래그 얻기
    if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
    {
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "setSocketNonBloking/fcntl/F_GETFL", StrUtil::strError(m_nLastError));
        return -1;
    }

    // nonblocking 설정
    flags |= O_NONBLOCK;

    // 파일디스크립터 플래그 설정
    if (fcntl(sockfd, F_SETFL, flags) < 0)
    {
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "setSocketNonBloking/fcntl/F_SETFL", StrUtil::strError(m_nLastError));
        return -1;
    }
#endif

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setSocketNonDelay()
/// @brief  Nagle 알고리즘 사용암함
/// @return 성공여부
///////////////////////////////////////////////////////////////////////////////
int JSocket::setSocketNonDelay()
{
	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return -1;
	}

    int on = 0;

#if defined(_hpux) || defined(hpux) || defined(__hpux)
    int on_len;
#elif defined(_SCO_DS) || defined(__USLC__)
    size_t on_len;
#else
    socklen_t on_len;
#endif
    on_len = sizeof(on);
    
    // 기존에 설정되어 있는지 확인
#if defined(_WIN32)
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&on, &on_len) < 0) 
#else
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &on, &on_len) < 0) 
#endif
	{
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "setSocketNonDelay/getsockopt/TCP_NODELAY", StrUtil::strError(m_nLastError));
        return -1;
    }
    
    // 이미 설정되어 있으면 리턴
    if (on == 1) return 0;
    
    on = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on)) != 0)
    {
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "setSocketNonDelay/setsockopt/TCP_NODELAY", StrUtil::strError(m_nLastError));

        return -1;
    }
    
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setSocketTos(bool isInteractive)
/// @brief  IP_TOS 설정
/// @return 성공여부
///////////////////////////////////////////////////////////////////////////////
int JSocket::setSocketTos(bool isInteractive)
{
#if defined(IP_TOS) && !defined(IP_TOS_IS_BROKEN)
    int tos = isInteractive ? IPTOS_LOWDELAY : IPTOS_THROUGHPUT;

	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return -1;
	}

    if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) != 0)
    {
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "setSocketTos/setsockopt/IP_TOS", StrUtil::strError(m_nLastError));
        return -1;
    }
#endif
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::nbconnect(const struct sockaddr *serv_addr, socklen_t addrlen, int sec)
/// @brief  소켓에 연결하기
/// @note   타임아웃 지원 소켓에 연결하기
/// @param  serv_addr   [in] 연결할 서버 주소
/// @param  addrlen     [in] 주소 길이
/// @param  sec         [in] 타임아웃 초
/// @return 성공 여부 (성공: 0, 실패: -1)
///////////////////////////////////////////////////////////////////////////////
int JSocket::nbconnect(const struct sockaddr *serv_addr,
                        socklen_t addrlen, int sec)
{
    fd_set rfds, wfds;
    struct timeval tv;

	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return -1;
	}

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_SET(sockfd, &rfds);
    FD_SET(sockfd, &wfds);

    // 타임아웃 설정
    tv.tv_sec = sec;
    tv.tv_usec = 0;

    // 넌블러킹 소켓 설정
    setSocketNonBloking();

    int retval = connect(sockfd, serv_addr, addrlen);

    // connect completed immediately
	if (retval == 0)
	{
        m_isConnected = true;
        return 0;
	}
#if defined(_WIN32)
	else if ((WSAGetLastError() == WSAEINPROGRESS) || (WSAGetLastError() == WSAEWOULDBLOCK))
#else
	else if ((errno == EINPROGRESS) || (errno == EAGAIN))
#endif
	{
		// 넌블럭킹 상태이므로 소켓I/O을 타임아웃까지 체크하고 시그널이 있을경우 에러 값을 처리

		// 읽기/쓰기 버퍼 검사
		int state = select((int)sockfd+1, &rfds, &wfds, 0, sec ? &tv : NULL);

		// select 에러
		if (state < 0) 
		{
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "nbconnect/select", StrUtil::strError(m_nLastError));
			return -1;
		}
		// select 타임아웃
		else if (state == 0)
		{
#if defined(_WIN32)
			errno = WSAETIMEDOUT;
#else
			errno = ETIMEDOUT;
#endif
			m_nLastError = errno;
			m_strLastErrorString.format("%s::%s", "nbconnect/select", "Connection timed out");
			return -1;
		}

		// 읽기 신호 들어옴
		else if (state > 0)
		{
			if (FD_ISSET(sockfd, &rfds) || FD_ISSET(sockfd, &wfds))
			{
				int error = 0;

#if defined(_hpux) || defined(hpux) || defined(__hpux)
				int err_len;
#elif defined(_SCO_DS) || defined(__USLC__)
				size_t err_len;
#else
				socklen_t err_len;
#endif

				err_len = sizeof(error);
#if defined(_WIN32)
				if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &err_len) < 0) 
#else
				if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0) 
#endif
				{
					m_nLastError = _geterrno();
					m_strLastErrorString.format("%s::%s", "nbconnect/getsockopt/SO_ERROR", StrUtil::strError(m_nLastError));
					return -1;
				}

				// 에러 발생이면 리턴
				if (error != 0)
				{
					m_nLastError = errno = error; // 소켓정보에서 얻은 에러값을 설정
					m_strLastErrorString.format("%s::%s", "nbconnect/error", StrUtil::strError(m_nLastError));
					return -1;
				}
	            
				m_isConnected = true;
				return 0;
			}
		}
	}
	else
	{
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "nbconnect/connect", StrUtil::strError(m_nLastError));
        return -1;
	}

    return -1;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::nbconnect(const char* addr, int port, int srcport, int sec)
/// @brief  소켓에 연결하기
/// @note   타임아웃 지원 소켓에 연결하기
/// @param  addr        [in] 연결할 서버 주소
/// @param  port        [in] 포트
/// @param  srcport     [in] 소스(로컬) 포트
/// @param  sec         [in] 타임아웃 초
/// @return 성공 여부 (성공: 0, 실패: -1)
///////////////////////////////////////////////////////////////////////////////
int JSocket::nbconnect(const char* addr, int port, int srcport, int sec)
{
    struct sockaddr_in so_addr;
    sockfd_t sockfd;
#if defined(_hpux) || defined(hpux) || defined(__hpux)
    int addr_len;
#elif defined(_SCO_DS) || defined(__USLC__)
    size_t addr_len;
#else
    socklen_t addr_len;
#endif
    addr_len = sizeof(so_addr);

	clearError();

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "nbconnect/socket", StrUtil::strError(m_nLastError));
		return -1;
    }

    // 로컬 포트를 특별히 지정해야 될 경우
    // 예) FTP Active Mode에서 데이터 보내는 포트를 20번으로 고정해야 되는 경우
    if (srcport > 0) 
    {
        int on = 1;

        // 소켓 옵션 설정
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) != 0)
        {
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "nbconnect/setsockopt/SO_REUSEADDR", StrUtil::strError(m_nLastError));

			closefd(sockfd);
            return -1;
        }
        
        // 소스(로컬) 정보 설정
        so_addr.sin_family = AF_INET;
        so_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        so_addr.sin_port = htons(srcport);

        if (bind(sockfd, (struct sockaddr *)&so_addr, sizeof(so_addr)) < 0)
        {
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "nbconnect/bind", StrUtil::strError(m_nLastError));

			closefd(sockfd);
            return -1;
        }
    }

    // 접속 주소 설정
    so_addr.sin_family = AF_INET;
    so_addr.sin_addr.s_addr = inet_addr(addr);
    so_addr.sin_port = htons(port);

    // 소켓(sock) 정보 설정
    setSockInfo(sockfd, addr, port);
    // 소켓(keep alive) 정보 설정
    setKeepAlive(AF_INET);

    return nbconnect((struct sockaddr *)&so_addr, addr_len, sec);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::writeSocket(const void* msg, size_t len)
/// @brief  타임아웃 지원 소켓에 쓰기
/// @param  msg         [in] 소켓에 쓸 내용
/// @param  len         [in] 내용의 길이
/// @return 성공: >=0 (보낸 길이)\n
///         실패: -1
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::writeSocket(const void* msg, size_t len)
{
again:
    if (m_isEof) return 0;
    // 소켓이 끊겼는지 검사
    if (isReceiveEof()) return 0;
	
	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return -1;
	}

    // 넌블러킹 소켓에 쓰기
    errno = 0;
#if defined(_WIN32)
    ssize_t nwritten = send(sockfd, (const char*)msg, (int)len, 0);
#else
    ssize_t nwritten = send(sockfd, msg, len, 0);
#endif

    if (nwritten < 0)
    {
        // 쓰기전에 인터럽트가 발생했으면 다시 쓰기
#if defined(_WIN32)
        if (errno == WSAEINTR) goto again;
#else
        if (errno == EINTR) goto again;
#endif
        // 즉시 데이터를 쓸 소켓 공간이 없을때는 소켓 버퍼에 데이터가 보내지는지 검사한다.
#if defined(_WIN32)
        if ((errno == WSAEWOULDBLOCK) || (errno == WSAEAGAIN))
#else
        if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
#endif
        {
            if (isWriteable()) goto again;
        }

		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "writeSocket/send", StrUtil::strError(m_nLastError));
    }

    return nwritten;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::readSocket(void* buf, size_t len, int flag)
/// @brief  타임아웃 지원 소켓에 읽기
/// @param  buf         [in] 내용을 저장할 버퍼
/// @param  len         [in] 읽을 내용 길이
/// @param  flag        [in] 일기 모드 (MSG_OOB, MSG_PEEK, MSG_WAILALL)
/// @return 성공: >=0 (읽은 길이)\n
///         실패: -1
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::readSocket(void* buf, size_t len, int flag)
{
    if (!isReadable()) return -1;

again:

    if (m_isEof) return 0;
	
	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return -1;
	}
	
    // 넌블러킹 소켓에서 읽기
    errno = 0;
#if defined(_WIN32)
    ssize_t nread = recv(sockfd, (char*)buf, (int)len, flag);
#else
    ssize_t nread = recv(sockfd, buf, len, flag);
#endif
		
    if (nread < 0)
    {
        // 읽기전에 인터럽트가 발생했으면 다시 읽기
#if defined(_WIN32)
        if (errno == WSAEINTR) goto again;
#else
        if (errno == EINTR) goto again;
#endif
        // 즉시 데이터를 읽을 소켓 공간이 없을때는 소켓 버퍼에 데이터가 읽어지는지 검사한다.
#if defined(_WIN32)
        if ((errno == WSAEWOULDBLOCK) || (errno == WSAEAGAIN))
#else
        if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
#endif
        {
            if (isReadable()) goto again;
        }

		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "readSocket/recv", StrUtil::strError(m_nLastError));
    }

    return nread;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isWriteable()
/// @brief  쓰기 가능한지(타임아웃) 체크
/// @return 가능 여부
///////////////////////////////////////////////////////////////////////////////
bool JSocket::isWriteable()
{
    fd_set wfds, efds;
    struct timeval tv;

	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return false;
	}
	
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    FD_SET(sockfd, &wfds);
    FD_SET(sockfd, &efds);

    // 타임아웃 설정
    tv.tv_sec = m_nTimeout;
    tv.tv_usec = 0;

    // 쓰기와 에러 버퍼 검사
	int state = select((int)sockfd+1, 0, &wfds, &efds, m_nTimeout ? &tv : NULL);
    
    // select 에러
    if (state < 0) 
	{
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "isWriteable/select", StrUtil::strError(m_nLastError));

		return false;
	}
    // select 타임아웃
    else if (state == 0)
    {
#if defined(_WIN32)
        errno = WSAETIMEDOUT;
#else
        errno = ETIMEDOUT;
#endif
		m_nLastError = errno;
		m_strLastErrorString.format("%s::%s", "isWriteable/select", "Connection timed out");

		return false;
    }
    // 쓰기 신호 들어옴
    else if (state > 0)
    {
        if (FD_ISSET(sockfd, &wfds))
        {
            int error = 0;

#if defined(_hpux) || defined(hpux) || defined(__hpux)
            int err_len;
#elif defined(_SCO_DS) || defined(__USLC__)
            size_t err_len;
#else
            socklen_t err_len;
#endif
            err_len = sizeof(error);

#if defined(_WIN32)
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &err_len) < 0) 
#else
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0) 
#endif
            {
				m_nLastError = _geterrno();
				m_strLastErrorString.format("%s::%s", "isWriteable/getsockopt/SO_ERROR", StrUtil::strError(m_nLastError));

				return false;
            }

			// 에러 발생이면 리턴
			if (error != 0)
			{
				m_nLastError = errno = error; // 소켓정보에서 얻은 에러값을 설정
				m_strLastErrorString.format("%s::%s", "isWriteable/error", StrUtil::strError(m_nLastError));
				return false;
			}

            return true;
        }
        
		if (FD_ISSET(sockfd, &efds))
		{
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "isWriteable/FD_ISSET", StrUtil::strError(m_nLastError));

			return false;
		}
    } // state == 0

    return false;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isReadable()
/// @brief  읽기 가능한지(타임아웃) 체크
/// @return 가능 여부
///////////////////////////////////////////////////////////////////////////////
bool JSocket::isReadable()
{
    fd_set rfds, efds;
    struct timeval tv;

	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return false;
	}

    FD_ZERO(&rfds);
    FD_ZERO(&efds);
    FD_SET(sockfd, &rfds);
    FD_SET(sockfd, &efds);

    // 타임아웃 설정
    tv.tv_sec = m_nTimeout;
    tv.tv_usec = 0;

    // 읽기와 에러 버퍼 검사
	int state = select((int)sockfd+1, &rfds, 0, &efds, m_nTimeout ? &tv : NULL);

    // select 에러
    if (state < 0) 
	{
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "isReadable/select", StrUtil::strError(m_nLastError));

		return false;
	}
    // select 타임아웃
    else if (state == 0)
    {
#if defined(_WIN32)
        errno = WSAETIMEDOUT;
#else
        errno = ETIMEDOUT;
#endif
		m_nLastError = errno;
		m_strLastErrorString.format("%s::%s", "isReadable/select", "Connection timed out");

		return false;
    }
    // 읽기 신호 들어옴
    else if (state > 0)
    {
        if (FD_ISSET(sockfd, &rfds))
        {
            int error = 0;

#if defined(_hpux) || defined(hpux) || defined(__hpux)
            int err_len;
#elif defined(_SCO_DS) || defined(__USLC__)
            size_t err_len;
#else
            socklen_t err_len;
#endif
            err_len = sizeof(error);

#if defined(_WIN32)
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &err_len) < 0) 
#else
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0) 
#endif
            {
				m_nLastError = _geterrno();
				m_strLastErrorString.format("%s::%s", "isWriteable/getsockopt/SO_ERROR", StrUtil::strError(m_nLastError));
				
                return false;
            }
            
			// 에러 발생이면 리턴
			if (error != 0)
			{
				m_nLastError = errno = error; // 소켓정보에서 얻은 에러값을 설정
				m_strLastErrorString.format("%s::%s", "isReadable/error", StrUtil::strError(m_nLastError));

				return false;
			}

            return true;
        }
		
        if (FD_ISSET(sockfd, &efds))
		{
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "isReadable/FD_ISSET", StrUtil::strError(m_nLastError));
			
			return false;
		}
    } // state == 0

    return false;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isReceiveEof()
/// @brief  EOF 신호가 있는지 체크
/// @return 가능 여부
///////////////////////////////////////////////////////////////////////////////
bool JSocket::isReceiveEof()
{
    fd_set rfds, efds;
    struct timeval tv;

	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return false;
	}
	
    FD_ZERO(&rfds);
    FD_ZERO(&efds);
    FD_SET(sockfd, &rfds);
    FD_SET(sockfd, &efds);

    // 타임아웃 설정
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    // 쓰기와 에러 버퍼 검사
    int state = select((int)sockfd+1, &rfds, 0, &efds, &tv);

    // select 에러
    if (state < 0) 
	{
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "isReceiveEof/select", StrUtil::strError(m_nLastError));

		return false;
	}
    // select 타임아웃
    else if (state == 0)
    {
#if defined(_WIN32)
        errno = WSAETIMEDOUT;
#else
        errno = ETIMEDOUT;
#endif
		m_nLastError = errno;
		m_strLastErrorString.format("%s::%s", "isReceiveEof/select", "Connection timed out");

        return false;
    }
    // 읽기 신호 들어옴
    else if (state > 0)
    {
        if (FD_ISSET(sockfd, &rfds))
        {
            int error = 0;

#if defined(_hpux) || defined(hpux) || defined(__hpux)
            int err_len;
#elif defined(_SCO_DS) || defined(__USLC__)
            size_t err_len;
#else
            socklen_t err_len;
#endif
            err_len = sizeof(error);

#if defined(_WIN32)
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &err_len) < 0) 
#else
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0) 
#endif
            {
				m_nLastError = _geterrno();
				m_strLastErrorString.format("%s::%s", "isReceiveEof/getsockopt/SO_ERROR", StrUtil::strError(m_nLastError));

                return true;
            }
            
            // 에러 발생이면 리턴
            if (error)
            {
				m_nLastError = errno = error; // 소켓정보에서 얻은 에러값을 설정
				m_strLastErrorString.format("%s::%s", "isReceiveEof/error", StrUtil::strError(m_nLastError));

                return true;
            }

            // 소켓이 끊겼는지 검사
            char peek;
            int nread = recv(sockfd, &peek, 1, MSG_PEEK);

            if (nread == 0) return true;
            return false;
        }

		if (FD_ISSET(sockfd, &efds))
		{
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "isReceiveEof/FD_ISSET", StrUtil::strError(m_nLastError));
			
			return true;
		}

    } // state == 0

    return false;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::writeFixedSize(const void* msg, size_t len)
/// @brief  버퍼 크기만큼 보내기
/// @param  msg         [in] 소켓에 쓸 내용
/// @param  len         [in] 내용의 길이
/// @return 성공: >=0 (보낸 길이)\n
///         실패: -1
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::writeFixedSize(const void* msg, size_t len)
{
    ssize_t nwritten = 0;
    ssize_t ntotal = 0, nleft = (ssize_t)len;
    const void *ptr = msg;

	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return 0;
	}

	
    // 버퍼 크기만큼 반드시 보낸다.
    while (nleft > 0) {
        nwritten = writeSocket(ptr, nleft);

        if (nwritten <= 0)
        {
            if (m_isTimeoutIsError) setEof(true);
            else 
            {
#if defined(_WIN32)
                if (errno != WSAETIMEDOUT) setEof(true);
#else
                if (errno != ETIMEDOUT) setEof(true);
#endif
            }
            if (ntotal > 0) break;
            else return -1;
        }

        ntotal += nwritten;
        nleft  -= nwritten;
        ptr     = (unsigned char*)ptr + nwritten;
    }

    return ntotal;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::readFixedSize(void* buf, size_t len)
/// @brief  버퍼 크기만큼 읽기
/// @param  buf     [out] 내용을 저장할 버퍼
/// @param  len     [in] 읽을 내용 길이
/// @return 성공: >=0 (보낸 길이)\n
///         실패: -1
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::readFixedSize(void* buf, size_t len)
{
    ssize_t nread = 0;
    ssize_t ntotal = 0, nleft = (ssize_t)len;
    void *ptr = buf;

	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return 0;
	}
	
    // 버퍼 크기만큼 반드시 읽는다.
    while (nleft > 0)
    {
        nread = readSocket(ptr, nleft);

		if (nread <= 0)
        {
            if (m_isTimeoutIsError) setEof(true);
            else 
            {
#if defined(_WIN32)
                if (errno != WSAETIMEDOUT) setEof(true);
#else
                if (errno != ETIMEDOUT) setEof(true);
#endif
            }
            if (ntotal > 0) break;
            else return -1;
        }

        ntotal += nread;
        nleft  -= nread;
        ptr = (unsigned char*)ptr + nread;
    }

    return ntotal;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::readLine(void* buf, size_t len)
/// @brief  한줄 읽기
/// @param  buf     [out] 내용을 저장할 버퍼
/// @param  len     [in] 읽을 내용 길이
/// @return 성공: >= 0 (읽은 길이, 0: 소켓 종료)\n
///         실패: -1
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::readLine(void* buf, size_t len)
{
    size_t nread = 0;
    void *newline = 0, *ptr = buf;
    size_t ntotal = 0, nleft = len;

	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return 0;
	}
    
    if (len <= 0) return 0; 

    do 
    {
        // 미리 읽어서 LF확인하기
        if ((nread = readSocket(ptr, nleft, MSG_PEEK)) <= 0)
        {
            if (m_isTimeoutIsError) setEof(true);
            else 
            {
#if defined(_WIN32)
                if (errno != WSAETIMEDOUT) setEof(true);
#else
                if (errno != ETIMEDOUT) setEof(true);
#endif
            }
            if (ntotal > 0) break;
            else return -1;
        }
        
        if ((newline = memchr(ptr, '\n', nread)) != NULL)
            nread = (unsigned char*)newline - (unsigned char*)ptr + 1;

        // 실제 읽기
        if ((nread = readSocket(ptr, nread)) <= 0)
        {
            if (m_isTimeoutIsError) setEof(true);
            else 
            {
#if defined(_WIN32)
                if (errno != WSAETIMEDOUT) setEof(true);
#else
                if (errno != ETIMEDOUT) setEof(true);
#endif
            }
            if (ntotal > 0) break;
            else return -1;
        }

        ntotal += nread;
        nleft  -= nread;

        ptr = (unsigned char*)ptr + nread;
    } while (!newline && nleft);

    return (ssize_t)ntotal;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::nbsend(const void* msg, size_t len)
/// @brief  소켓에 쓰기
/// @note   타임아웃 지원 소켓에 보내기
/// @param  msg     [in] 소켓에 쓸 내용
/// @param  len     [in] 내용의 길이
/// @return 성공: >=0 (보낸 길이)\n
///         실패: < 0 (쓰기에러(-1), 타임아웃(-2), 연결끊김(-3))
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::nbsend(const void* msg, size_t len)
{
    if (isEof()) return -3;

    ssize_t nwritten = writeFixedSize(msg, len);

    if (nwritten <= 0)
    {
#if defined(_WIN32)
        if (errno == WSAETIMEDOUT) return -2;
#else
        if (errno == ETIMEDOUT) return -2;
#endif
        return -1;
    }

    return nwritten;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::nbsend(const bstring* bstr)
/// @brief  소켓에 쓰기
/// @note   타임아웃 지원 소켓에 보내기
/// @param  bstr    [in] 소켓에 쓸 버퍼
/// @return 성공: >=0 (보낸 길이)\n
///         실패: < 0 (쓰기에러(-1), 타임아웃(-2), 연결끊김(-3))
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::nbsend(const bstring* bstr)
{
    return nbsend(bstr->data(), bstr->size());
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::nbreceive(void* buf, size_t len, int mode)
/// @brief  소켓에서 읽기
/// @note   타임아웃 지원 소켓에서 받기
/// @param  buf     [out] 내용을 저장할 버퍼
/// @param  len     [in] 읽을 내용 길이
/// @param  mode    [in] 읽기 방식 (일반(기본값), 라인, 고정길이)
/// @return 성공: >=0 (읽은 길이)\n
///         실패: < 0 (읽기에러(-1), 타임아웃(-2), 연결끊김(-3))
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::nbreceive(void* buf, size_t len, int mode)
{
    if (isEof()) return -3;

    ssize_t nread = 0;

    switch(mode)
    {
        case R_NORMAL:  nread = readSocket(buf, len);       break;
        case R_LINE:    nread = readLine(buf, len);         break;
        case R_FIXED:   nread = readFixedSize(buf, len);    break;
    }

    if (nread <= 0)
    {
#if defined(_WIN32)
        if (errno == WSAETIMEDOUT) return -2;
#else
        if (errno == ETIMEDOUT) return -2;
#endif
        return -1;
    }

    return nread;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::nbreceive(bstring* bstr, size_t len, int mode)
/// @brief  소켓에서 읽기
/// @note   타임아웃 지원 소켓에서 받기
/// @param  bstr    [out] 내용을 저장할 버퍼
/// @param  len     [in] 읽을 내용 길이
/// @param  mode    [in] 읽기 방식 (일반(기본값), 라인, 고정길이)
/// @return 성공: >=0 (읽은 길이)\n
///         실패: < 0 (읽기에러(-1), 타임아웃(-2), 연결끊김(-3))
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::nbreceive(bstring* bstr, size_t len, int mode)
{
    bstr->allocation(len);

    ssize_t nread = nbreceive(bstr->dataptr(), len, mode);

    if (nread <= 0)
    {
        if (m_isTimeoutIsError) setEof(true);
        else 
        {
#if defined(_WIN32)
            if (errno != WSAETIMEDOUT) setEof(true);
#else
            if (errno != ETIMEDOUT) setEof(true);
#endif
        }
        return nread;
    }
    
    (*bstr)[nread] = '\0';
    bstr->setsize(nread);
    
    return nread;   
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::nbreceive()
/// @brief  소켓에서 받아서 버퍼링
/// @return 성공: true (EOF, BUF_MAX)
///         실패: false
///////////////////////////////////////////////////////////////////////////////
bool JSocket::nbreceive()
{
    byte_t buf[IO_BUF_MAX+1];
    memset(buf, 0, IO_BUF_MAX+1);
    
    int nread = readSocket(buf, IO_BUF_MAX);
    
    if (nread <= 0)
    {
        if (m_isTimeoutIsError) setEof(true);
        else 
        {
#if defined(_WIN32)
            if (errno != WSAETIMEDOUT) setEof(true);
#else
            if (errno != ETIMEDOUT) setEof(true);
#endif
        }
        return true;
    }
    
    // 버퍼에 추가하기
    m_bstrBuffer.append(buf, nread);

    return false;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring* JSocket::getBuffer()
/// @brief  버퍼 얻기
/// @return 버퍼
///////////////////////////////////////////////////////////////////////////////
bstring* JSocket::getBuffer()
{
    return &m_bstrBuffer;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::getBuffer(bstring* bstr)
/// @brief  버퍼 얻기
/// @param  bstr    [out] 내용을 저장할 버퍼
///////////////////////////////////////////////////////////////////////////////
void JSocket::getBuffer(bstring* bstr)
{
    *bstr = m_bstrBuffer;
}

__END_NAMESPACE_JFX
