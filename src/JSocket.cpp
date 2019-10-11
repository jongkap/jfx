// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       JSocket.cpp
/// @brief      JSocket Ŭ���� ����
/// @author     Jongkap Jeong <jongkap@mail.com>
/// @date       2008/06/09 - ó���ۼ�
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

#define IO_BUF_MAX 1024  ///< Ȯ���� IO ���� ũ��


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

    // ����(peer) ���� ����
    setPeerInfo(sockfd, domain);
    
    // ����(keep alive) ���� ����
    setKeepAlive(domain);

    // �ͺ�ŷ ���� ����
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
/// @brief  ���� Ÿ�Ӿƿ� ���� ó��
/// @param  isError [in] ���� ó�� ���� ���� (�⺻��: true)
///////////////////////////////////////////////////////////////////////////////
void JSocket::setTimeoutIsError(bool isError)
{
    m_isTimeoutIsError = isError;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isConnected()
/// @brief  ���� ���� ���
/// @return ���� ����
///////////////////////////////////////////////////////////////////////////////
bool JSocket::isConnected()
{
    return m_isConnected;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn sockfd_t JSocket::getSocketFile()
/// @brief  ���ؼ��� ���� ��ũ���� ���
/// @return ���� ���� ��ũ����
///////////////////////////////////////////////////////////////////////////////
sockfd_t JSocket::getSocketFile()
{
    return m_connInfo.sockfd;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::closeSocketFile()
/// @brief  ���ؼ��� ���� ��ũ���� �ݱ�
/// @return ���� ���� ��ũ����
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
/// @brief  ���� ���� IP ����
/// @return ���� ���� IP
///////////////////////////////////////////////////////////////////////////////
char* JSocket::getIpAddr()
{
    return m_connInfo.ip;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn char* JSocket::getPort()
/// @brief  ���� ���� ��Ʈ ���� ���
/// @return ���� ���� ��Ʈ
///////////////////////////////////////////////////////////////////////////////
int JSocket::getPort()
{
    return m_connInfo.port;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isEof()
/// @brief  EOF ���� ���
/// @return EOF ����
///////////////////////////////////////////////////////////////////////////////
bool JSocket::isEof()
{
    return m_isEof;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::setEof(bool isEof)
/// @brief  EOF ���� ����
/// @param  isEof       [in] EOF ���� (�⺻��: true)
/// @return EOF ����
///////////////////////////////////////////////////////////////////////////////
void JSocket::setEof(bool isEof)
{
    m_isEof = isEof;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::getTimeout()
/// @brief  Ÿ�Ӿƿ� �ð� ���
/// @return EOF ����
///////////////////////////////////////////////////////////////////////////////
int JSocket::getTimeout()
{
    return m_nTimeout;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::setTimeout(int nTimeout)
/// @brief  Ÿ�Ӿƿ� �ð� ����
/// @param  nTimeout    [in] Ÿ�Ӿƿ� �ð�
/// @brief  EOF ����
///////////////////////////////////////////////////////////////////////////////
void JSocket::setTimeout(int nTimeout)
{
    m_nTimeout = nTimeout;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bool JSocket::isReceiveTimeOver(int sec, time_t tTime)
/// @brief  Ÿ�ӿ��� üũ
/// @param  sec     [in] Ÿ�Ӿƿ� ��
/// @param  tTime   [in] �ð�
/// @return Ÿ�ӿ��� ����
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
/// @brief  ������ ���� �ð� ����
/// @param  tTime   [in] �ð�
///////////////////////////////////////////////////////////////////////////////
void JSocket::setLastReceiveTime(time_t tTime)
{
    m_connInfo.actionTime = tTime;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ConnectionInfo* JSocket::getConnectionInfo()
/// @brief  ���ؼ� ���� ���
/// @return ���ؼ� ����
///////////////////////////////////////////////////////////////////////////////
ConnectionInfo* JSocket::getConnectionInfo()
{
    return &m_connInfo;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::getLastError()
/// @brief  ���� ��ȣ ���
/// @return ���� ��ȣ
///////////////////////////////////////////////////////////////////////////////
int JSocket::getLastError()
{
    return m_nLastError;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn const char* JSocket::getLastErrorString()
/// @brief  ���� ��Ʈ�� ���
/// @return ���� ��Ʈ��
///////////////////////////////////////////////////////////////////////////////
const char* JSocket::getLastErrorString()
{
    return m_strLastErrorString.c_str();
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::clearError()
/// @brief  ���� �ڵ�/�޽��� �ʱ�ȭ
///////////////////////////////////////////////////////////////////////////////
void JSocket::clearError()
{
	m_nLastError = 0;
	m_strLastErrorString = "";
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setKeepAlive(int domain)
/// @brief  ����(keep alive) ���� ����
/// @param  domain  [in] ���� ������
/// @return �������� (����: 0, ����: -1)
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
    
    // socket ���� ���
    if (domain == AF_INET)
    {
        // socket option ����
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
/// @brief  ����(peer) ���� ���
/// @param  sockfd  [in] ���� ���� ��ũ����
/// @param  domain  [in] ���� ������
/// @return ��������
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
    
    // socket ���� ���
    if (domain == AF_INET)
    {
        if (getpeername(sockfd, (struct sockaddr*)&so_addr, &addr_len) != 0)
        {
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "setPeerInfo/getpeername", StrUtil::strError(m_nLastError));
            m_isEof = true;
        }
    }

    // ���ؼ� ���� �����ϱ�
    m_connInfo.sockfd       = sockfd;
    if (domain == AF_INET)  strcpy(m_connInfo.ip, inet_ntoa(so_addr.sin_addr));
    m_connInfo.port         = ntohs(so_addr.sin_port);
    m_connInfo.dateTime     = time(0);
    m_connInfo.actionTime   = m_connInfo.dateTime;

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setSockInfo(sockfd_t sockfd, const char* addr, int port)
/// @brief  ����(sock) ���� ���
/// @param  sockfd  [in] ���� ���� ��ũ����
/// @param  addr    [in] �ּ�
/// @param  port    [in] ��Ʈ
/// @return ��������
///////////////////////////////////////////////////////////////////////////////
int JSocket::setSockInfo(sockfd_t sockfd, const char* addr, int port)
{
    // ���ؼ� ���� �����ϱ�
    m_connInfo.sockfd       = sockfd;
    strcpy(m_connInfo.ip,   addr);
    m_connInfo.port         = port;
    m_connInfo.dateTime     = time(0);
    m_connInfo.actionTime   = m_connInfo.dateTime;

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int JSocket::setSocketNonBloking()
/// @brief  �ͺ�ŷ ���� ����
/// @return ��������
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
    // ���ϵ�ũ���� �÷��� ���
    if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
    {
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "setSocketNonBloking/fcntl/F_GETFL", StrUtil::strError(m_nLastError));
        return -1;
    }

    // nonblocking ����
    flags |= O_NONBLOCK;

    // ���ϵ�ũ���� �÷��� ����
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
/// @brief  Nagle �˰��� ������
/// @return ��������
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
    
    // ������ �����Ǿ� �ִ��� Ȯ��
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
    
    // �̹� �����Ǿ� ������ ����
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
/// @brief  IP_TOS ����
/// @return ��������
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
/// @brief  ���Ͽ� �����ϱ�
/// @note   Ÿ�Ӿƿ� ���� ���Ͽ� �����ϱ�
/// @param  serv_addr   [in] ������ ���� �ּ�
/// @param  addrlen     [in] �ּ� ����
/// @param  sec         [in] Ÿ�Ӿƿ� ��
/// @return ���� ���� (����: 0, ����: -1)
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

    // Ÿ�Ӿƿ� ����
    tv.tv_sec = sec;
    tv.tv_usec = 0;

    // �ͺ�ŷ ���� ����
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
		// �ͺ�ŷ �����̹Ƿ� ����I/O�� Ÿ�Ӿƿ����� üũ�ϰ� �ñ׳��� ������� ���� ���� ó��

		// �б�/���� ���� �˻�
		int state = select((int)sockfd+1, &rfds, &wfds, 0, sec ? &tv : NULL);

		// select ����
		if (state < 0) 
		{
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "nbconnect/select", StrUtil::strError(m_nLastError));
			return -1;
		}
		// select Ÿ�Ӿƿ�
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

		// �б� ��ȣ ����
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

				// ���� �߻��̸� ����
				if (error != 0)
				{
					m_nLastError = errno = error; // ������������ ���� �������� ����
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
/// @brief  ���Ͽ� �����ϱ�
/// @note   Ÿ�Ӿƿ� ���� ���Ͽ� �����ϱ�
/// @param  addr        [in] ������ ���� �ּ�
/// @param  port        [in] ��Ʈ
/// @param  srcport     [in] �ҽ�(����) ��Ʈ
/// @param  sec         [in] Ÿ�Ӿƿ� ��
/// @return ���� ���� (����: 0, ����: -1)
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

    // ���� ����
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "nbconnect/socket", StrUtil::strError(m_nLastError));
		return -1;
    }

    // ���� ��Ʈ�� Ư���� �����ؾ� �� ���
    // ��) FTP Active Mode���� ������ ������ ��Ʈ�� 20������ �����ؾ� �Ǵ� ���
    if (srcport > 0) 
    {
        int on = 1;

        // ���� �ɼ� ����
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) != 0)
        {
			m_nLastError = _geterrno();
			m_strLastErrorString.format("%s::%s", "nbconnect/setsockopt/SO_REUSEADDR", StrUtil::strError(m_nLastError));

			closefd(sockfd);
            return -1;
        }
        
        // �ҽ�(����) ���� ����
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

    // ���� �ּ� ����
    so_addr.sin_family = AF_INET;
    so_addr.sin_addr.s_addr = inet_addr(addr);
    so_addr.sin_port = htons(port);

    // ����(sock) ���� ����
    setSockInfo(sockfd, addr, port);
    // ����(keep alive) ���� ����
    setKeepAlive(AF_INET);

    return nbconnect((struct sockaddr *)&so_addr, addr_len, sec);
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::writeSocket(const void* msg, size_t len)
/// @brief  Ÿ�Ӿƿ� ���� ���Ͽ� ����
/// @param  msg         [in] ���Ͽ� �� ����
/// @param  len         [in] ������ ����
/// @return ����: >=0 (���� ����)\n
///         ����: -1
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::writeSocket(const void* msg, size_t len)
{
again:
    if (m_isEof) return 0;
    // ������ ������� �˻�
    if (isReceiveEof()) return 0;
	
	clearError();
	sockfd_t sockfd = getSocketFile();
    if (sockfd == ESOCKFD) 
	{
		m_nLastError = -1;
		m_strLastErrorString = "sockfd not found!";
		return -1;
	}

    // �ͺ�ŷ ���Ͽ� ����
    errno = 0;
#if defined(_WIN32)
    ssize_t nwritten = send(sockfd, (const char*)msg, (int)len, 0);
#else
    ssize_t nwritten = send(sockfd, msg, len, 0);
#endif

    if (nwritten < 0)
    {
        // �������� ���ͷ�Ʈ�� �߻������� �ٽ� ����
#if defined(_WIN32)
        if (errno == WSAEINTR) goto again;
#else
        if (errno == EINTR) goto again;
#endif
        // ��� �����͸� �� ���� ������ �������� ���� ���ۿ� �����Ͱ� ���������� �˻��Ѵ�.
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
/// @brief  Ÿ�Ӿƿ� ���� ���Ͽ� �б�
/// @param  buf         [in] ������ ������ ����
/// @param  len         [in] ���� ���� ����
/// @param  flag        [in] �ϱ� ��� (MSG_OOB, MSG_PEEK, MSG_WAILALL)
/// @return ����: >=0 (���� ����)\n
///         ����: -1
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
	
    // �ͺ�ŷ ���Ͽ��� �б�
    errno = 0;
#if defined(_WIN32)
    ssize_t nread = recv(sockfd, (char*)buf, (int)len, flag);
#else
    ssize_t nread = recv(sockfd, buf, len, flag);
#endif
		
    if (nread < 0)
    {
        // �б����� ���ͷ�Ʈ�� �߻������� �ٽ� �б�
#if defined(_WIN32)
        if (errno == WSAEINTR) goto again;
#else
        if (errno == EINTR) goto again;
#endif
        // ��� �����͸� ���� ���� ������ �������� ���� ���ۿ� �����Ͱ� �о������� �˻��Ѵ�.
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
/// @brief  ���� ��������(Ÿ�Ӿƿ�) üũ
/// @return ���� ����
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

    // Ÿ�Ӿƿ� ����
    tv.tv_sec = m_nTimeout;
    tv.tv_usec = 0;

    // ����� ���� ���� �˻�
	int state = select((int)sockfd+1, 0, &wfds, &efds, m_nTimeout ? &tv : NULL);
    
    // select ����
    if (state < 0) 
	{
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "isWriteable/select", StrUtil::strError(m_nLastError));

		return false;
	}
    // select Ÿ�Ӿƿ�
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
    // ���� ��ȣ ����
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

			// ���� �߻��̸� ����
			if (error != 0)
			{
				m_nLastError = errno = error; // ������������ ���� �������� ����
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
/// @brief  �б� ��������(Ÿ�Ӿƿ�) üũ
/// @return ���� ����
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

    // Ÿ�Ӿƿ� ����
    tv.tv_sec = m_nTimeout;
    tv.tv_usec = 0;

    // �б�� ���� ���� �˻�
	int state = select((int)sockfd+1, &rfds, 0, &efds, m_nTimeout ? &tv : NULL);

    // select ����
    if (state < 0) 
	{
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "isReadable/select", StrUtil::strError(m_nLastError));

		return false;
	}
    // select Ÿ�Ӿƿ�
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
    // �б� ��ȣ ����
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
            
			// ���� �߻��̸� ����
			if (error != 0)
			{
				m_nLastError = errno = error; // ������������ ���� �������� ����
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
/// @brief  EOF ��ȣ�� �ִ��� üũ
/// @return ���� ����
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

    // Ÿ�Ӿƿ� ����
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    // ����� ���� ���� �˻�
    int state = select((int)sockfd+1, &rfds, 0, &efds, &tv);

    // select ����
    if (state < 0) 
	{
		m_nLastError = _geterrno();
		m_strLastErrorString.format("%s::%s", "isReceiveEof/select", StrUtil::strError(m_nLastError));

		return false;
	}
    // select Ÿ�Ӿƿ�
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
    // �б� ��ȣ ����
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
            
            // ���� �߻��̸� ����
            if (error)
            {
				m_nLastError = errno = error; // ������������ ���� �������� ����
				m_strLastErrorString.format("%s::%s", "isReceiveEof/error", StrUtil::strError(m_nLastError));

                return true;
            }

            // ������ ������� �˻�
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
/// @brief  ���� ũ�⸸ŭ ������
/// @param  msg         [in] ���Ͽ� �� ����
/// @param  len         [in] ������ ����
/// @return ����: >=0 (���� ����)\n
///         ����: -1
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

	
    // ���� ũ�⸸ŭ �ݵ�� ������.
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
/// @brief  ���� ũ�⸸ŭ �б�
/// @param  buf     [out] ������ ������ ����
/// @param  len     [in] ���� ���� ����
/// @return ����: >=0 (���� ����)\n
///         ����: -1
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
	
    // ���� ũ�⸸ŭ �ݵ�� �д´�.
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
/// @brief  ���� �б�
/// @param  buf     [out] ������ ������ ����
/// @param  len     [in] ���� ���� ����
/// @return ����: >= 0 (���� ����, 0: ���� ����)\n
///         ����: -1
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
        // �̸� �о LFȮ���ϱ�
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

        // ���� �б�
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
/// @brief  ���Ͽ� ����
/// @note   Ÿ�Ӿƿ� ���� ���Ͽ� ������
/// @param  msg     [in] ���Ͽ� �� ����
/// @param  len     [in] ������ ����
/// @return ����: >=0 (���� ����)\n
///         ����: < 0 (���⿡��(-1), Ÿ�Ӿƿ�(-2), �������(-3))
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
/// @brief  ���Ͽ� ����
/// @note   Ÿ�Ӿƿ� ���� ���Ͽ� ������
/// @param  bstr    [in] ���Ͽ� �� ����
/// @return ����: >=0 (���� ����)\n
///         ����: < 0 (���⿡��(-1), Ÿ�Ӿƿ�(-2), �������(-3))
///////////////////////////////////////////////////////////////////////////////
ssize_t JSocket::nbsend(const bstring* bstr)
{
    return nbsend(bstr->data(), bstr->size());
}


///////////////////////////////////////////////////////////////////////////////
/// @fn ssize_t JSocket::nbreceive(void* buf, size_t len, int mode)
/// @brief  ���Ͽ��� �б�
/// @note   Ÿ�Ӿƿ� ���� ���Ͽ��� �ޱ�
/// @param  buf     [out] ������ ������ ����
/// @param  len     [in] ���� ���� ����
/// @param  mode    [in] �б� ��� (�Ϲ�(�⺻��), ����, ��������)
/// @return ����: >=0 (���� ����)\n
///         ����: < 0 (�б⿡��(-1), Ÿ�Ӿƿ�(-2), �������(-3))
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
/// @brief  ���Ͽ��� �б�
/// @note   Ÿ�Ӿƿ� ���� ���Ͽ��� �ޱ�
/// @param  bstr    [out] ������ ������ ����
/// @param  len     [in] ���� ���� ����
/// @param  mode    [in] �б� ��� (�Ϲ�(�⺻��), ����, ��������)
/// @return ����: >=0 (���� ����)\n
///         ����: < 0 (�б⿡��(-1), Ÿ�Ӿƿ�(-2), �������(-3))
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
/// @brief  ���Ͽ��� �޾Ƽ� ���۸�
/// @return ����: true (EOF, BUF_MAX)
///         ����: false
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
    
    // ���ۿ� �߰��ϱ�
    m_bstrBuffer.append(buf, nread);

    return false;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn bstring* JSocket::getBuffer()
/// @brief  ���� ���
/// @return ����
///////////////////////////////////////////////////////////////////////////////
bstring* JSocket::getBuffer()
{
    return &m_bstrBuffer;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn void JSocket::getBuffer(bstring* bstr)
/// @brief  ���� ���
/// @param  bstr    [out] ������ ������ ����
///////////////////////////////////////////////////////////////////////////////
void JSocket::getBuffer(bstring* bstr)
{
    *bstr = m_bstrBuffer;
}

__END_NAMESPACE_JFX
