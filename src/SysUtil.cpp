// $Id: $
///////////////////////////////////////////////////////////////////////////////
///
/// @file       SysUtil.cpp
/// @brief      SysUtil 클래스 구현
/// @author     Jongkap Jeong <jongkap@mail.com>
/// @date       2015/10/01 - First Release
///
/// Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
///
///////////////////////////////////////////////////////////////////////////////

#include <limits.h>

#if defined(_WIN32)
  #pragma warning(disable:4005)

  #include <Lmcons.h>
  #include <winsock2.h>
  #include <iphlpapi.h>

  #pragma comment(lib, "IPHLPAPI.lib")

  #define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
  #define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

  #ifndef snprintf
    #define snprintf _snprintf
  #endif

#else
  #include <unistd.h>
  #include <string.h>
  #include <pwd.h>
  #include <sys/param.h>
  #include <sys/utsname.h>
#endif

#if defined(__linux__) || defined(linux) || defined(__linux)
  #include <ifaddrs.h>
  #include <netpacket/packet.h>
#else

#endif


#include "jfx.h"

__BEGIN_NAMESPACE_JFX


#define MAX_MAC_ADDR_LEN 	12

#if defined(_WIN32)
  #define BUFSIZE 512
  static BOOL GetVersionValue(LPCTSTR key, LPTSTR version);
#endif

static int GetAllMacAddressBySDK(map<string, string>& mapMacAddr);
static int GetAllMacAddressByCMD(map<string, string>& mapMacAddr);

///////////////////////////////////////////////////////////////////////////////
/// @fn     int SysUtil::getOSPlatform(string& strPlatform)
/// @brief  OS 플랫폼 정보 얻기
/// @param  strPlatform    [out] 반환값
/// @return 성공 여부 (0: 성공, -1: 실패)
///////////////////////////////////////////////////////////////////////////////
int SysUtil::getOSPlatform(jstring& strPlatform)
{
#if defined(_WIN32)
	jstring buffer;
	TCHAR szOS[BUFSIZE];

	OSVERSIONINFO OS;
	OS.dwOSVersionInfoSize = sizeof(OS);

	GetVersionEx(&OS);

	switch (OS.dwPlatformId)
	{
		case 0:
			strPlatform += "Win3.1";
			break;
		case 1:
			switch (OS.dwMinorVersion)
			{
			case 0:
				strPlatform += "Win95";
				break;
			case 10:
				strPlatform += "Win98";
				break;
			case 98:
				strPlatform += "WinMe";
				break;
			}
			break;
		case 2:
			buffer.format("WinNT %d.%d", OS.dwMajorVersion, OS.dwMinorVersion);
			strPlatform += buffer;

			strPlatform += " ";
			getHostName(buffer);
			strPlatform += buffer;

			GetVersionValue("ProductName", szOS);

			buffer.format(" (%s)", szOS);
			strPlatform += buffer;

			if ((OS.dwMajorVersion <= 5) ||
				((OS.dwMajorVersion = 6) && (OS.dwMinorVersion < 1)))
			{
				GetVersionValue("BuildLab", szOS);
			}
			else
			{
				GetVersionValue("BuildLabEx", szOS);
			}

			buffer.format(" %s", szOS);
			strPlatform += buffer;
			break;
		default:
			strPlatform += "UNKNOWN";
			break;

	}

#else
	struct utsname retval;
	
	if(uname(&retval) < 0 ) strPlatform = "UNKNOWN";
	else
	{
		strPlatform = retval.sysname;
		strPlatform += " ";
		strPlatform += retval.nodename;
		strPlatform += " ";
		strPlatform += retval.release;
		strPlatform += " ";
		strPlatform += retval.version;
		strPlatform += " ";
		strPlatform += retval.machine;
	}
#endif	

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int SysUtil::getOSUserName(string& strOsUserName)
/// @brief  OS 사용자 명 얻기
/// @param  strOsUserName    [out] 반환값
/// @return 성공 여부 (0: 성공, -1: 실패)
///////////////////////////////////////////////////////////////////////////////
int SysUtil::getOSUserName(jstring& strOsUserName)
{
#if defined(_WIN32)
	char user_name[UNLEN+1];
	DWORD user_name_size = UNLEN + 1;

	if (GetUserName(user_name, &user_name_size))
		strOsUserName = user_name;
#else
	struct passwd *pw = getpwuid (geteuid());
	
	strOsUserName = pw->pw_name;
#endif

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn     int SysUtil::getHostName(jstring& strHostName)
/// @brief  호스트명 얻기
/// @param  strHostName    [out] 반환값
/// @return 성공 여부 (0: 성공, -1: 실패)
///////////////////////////////////////////////////////////////////////////////
int SysUtil::getHostName(jstring& strHostName)
{
#if defined(_WIN32)
	TCHAR  szComputerName[MAX_COMPUTERNAME_LENGTH+1];
	DWORD  lnNameLength= MAX_COMPUTERNAME_LENGTH;

	if (GetComputerName( szComputerName, &lnNameLength))
		strHostName = szComputerName;
	else
		strHostName = "UNKNOWN";
#else
	struct utsname retval;
	
	if(uname(&retval) < 0 ) strHostName = "UNKNOWN";
	else
	{
		strHostName = retval.nodename;
	}
#endif

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int SysUtil::getProcessName(string& strProcName)
/// @brief  프로세스 명 얻기
/// @param  strProcName    [out] 반환값
/// @return 성공 여부 (0: 성공, -1: 실패)
///////////////////////////////////////////////////////////////////////////////
int SysUtil::getProcessName(jstring& strProcName)
{
#if defined(_WIN32)
	char result[MAX_PATH] = {0, };
	GetModuleFileName(NULL, result, MAX_PATH);

	strProcName = result;

#elif defined(__linux__) || defined(linux) || defined(__linux)

 	char result[PATH_MAX] = {0, };
	readlink("/proc/self/exe", result, PATH_MAX);
	
	strProcName = result;
	
#elif defined(_AIX)

#elif defined(sun) || defined(__sun)
// Linux
// :
// /proc/<pid>/exe

 // Solaris:
 // /proc/<pid>/object/a.out (filename only)
 // /proc/<pid>/path/a.out (complete pathname)

// *BSD (and maybe Darwing too):
// /proc/<pid>/file


#elif defined(_hpux) || defined(hpux) || defined(__hpux)
  	char result[PATH_MAX];
  	struct pst_status ps;
  	
//	if (pstat_getproc( &ps, sizeof( ps ), 0, getpid() ) < 0)
//	return std::string();

//	if (pstat_getpathname( result, PATH_MAX, &ps.pst_fid_text ) < 0)
//		return std::string();

//	return std::string( result );
#else
	strProcName = "UNKNOWN";
#endif

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int SysUtil::getCurrentDir(string& strCurrentDir)
/// @brief  현재 디렉토리 얻기
/// @param  strCurrentDir    [out] 반환값
/// @return 성공 여부 (0: 성공, -1: 실패)
///////////////////////////////////////////////////////////////////////////////
int SysUtil::getCurrentDir(jstring& strCurrentDir)
{
#if defined(_WIN32)
	TCHAR buffer[MAX_PATH];

    if (GetCurrentDirectory(MAX_PATH, buffer) == 0)
    {
		strCurrentDir = "UNKNOWN";
		return -1;
	}
	
	strCurrentDir = buffer;
#else
	char buffer[MAXPATHLEN];
	
	if (getcwd(buffer,MAXPATHLEN) == NULL)
	{
		strCurrentDir = "UNKNOWN";
		return -1;
	}
	
	strCurrentDir = buffer;
#endif

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int SysUtil::getMacAddress(string& strMacAddr)
/// @brief  MAC Address 얻기
/// @param  strMacAddr    [out] 반환값
/// @return 성공 여부 (0: 성공, -1: 실패)
///////////////////////////////////////////////////////////////////////////////
int SysUtil::getMacAddress(jstring& strMacAddr)
{
	map<string, string> mapMacAddr;

	SysUtil::getAllMacAddress(mapMacAddr);

	if (mapMacAddr.size() > 0)
		strMacAddr = mapMacAddr.begin()->first;
	else strMacAddr.clear();

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
/// @fn int SysUtil::getAllMacAddress(map<string, string>& mapMacAddr);
/// @brief  모든 MAC Address 얻기
/// @param  mapMacAddr    [out] 반환값 (MAC주소, 인터페이스명)
/// @return 성공 여부 (0: 성공, -1: 실패)
///////////////////////////////////////////////////////////////////////////////
int SysUtil::getAllMacAddress(map<string, string>& mapMacAddr)
{
	if (GetAllMacAddressBySDK(mapMacAddr) <= 0)
		return GetAllMacAddressByCMD(mapMacAddr);

	return 0;
}


// SDK로부터 MAC정보 얻어옴
int GetAllMacAddressBySDK(map<string, string>& mapMacAddr)
{
#if defined(_WIN32)

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	mapMacAddr.clear();

	ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) return -1;

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) return -1;
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
	{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
			char buffer[32] = {0, };
			for (i = 0; i < pAdapter->AddressLength; i++) 
			{
				snprintf(buffer+(i*2), 2, "%02X", (int) pAdapter->Address[i]);
			}

			mapMacAddr[buffer] = pAdapter->AdapterName;

            pAdapter = pAdapter->Next;
        }
    } 

    if (pAdapterInfo) FREE(pAdapterInfo);

#elif defined(__linux__) || defined(linux) || defined(__linux)
	struct ifaddrs *ifaddr=NULL;
	struct ifaddrs *ifa = NULL;
	int i = 0;

	if (getifaddrs(&ifaddr) == -1) 
	{
		return -1;
	}
	else
	{
		for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
		{
			if ( (ifa->ifa_addr) && (ifa->ifa_addr->sa_family == AF_PACKET) )
			{
				struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
				
				char buffer[32] = {0, };
				for (i=0; i <s->sll_halen; i++)
				{
					snprintf(buffer+(i*2), 2, "%02X", s->sll_addr[i]);
				}

				mapMacAddr[buffer] = ifa->ifa_name;
			}
		}
		freeifaddrs(ifaddr);
	}
#else

#endif

	return (int)mapMacAddr.size();
}

// 커맨드 실행하여 MAC 주소 가져옴
int execute_gec_mac_command(const char* cmd, const char* grep, int if_pos, map<string, string>& mapMacAddr)
{
	FILE *in = NULL;
	char buff[512] = {0, };

	jstring if_name;

	mapMacAddr.clear();

#if defined(_WIN32)
	if (!(in = _popen(cmd, "r"))) {
#else
	if (!(in = popen(cmd, "r"))) {
#endif
		return -1;	
	}
	
	while(fgets(buff, sizeof(buff), in) != NULL) {
		int i = 0, col = 0;
		char buffer[32] = {0, };
		char *cptr = strstr(buff, grep);
		if (cptr == NULL) continue;

		if (strlen(cptr) > strlen(grep)) cptr += strlen(grep);
		else continue;

		if (strlen(cptr) < MAX_MAC_ADDR_LEN) continue;
		
		while (*cptr != '\0')
		{
			if (isxdigit(*cptr)) break; 
			++cptr;
		}
		
		// MAC 주소 추출
		for (i = 0; i < (int)strlen(cptr); i++)
		{
			if ((cptr[i] == ' ') || (cptr[i] == '\t')
				|| (cptr[i] == '\r') || (cptr[i] == '\n')) break;

			if (isxdigit(cptr[i])) 
			{
				snprintf(buffer+col, 1, "%c", cptr[i]);
				col++;
			}
		}
		
		// 인터페이스명 추출
		if (if_pos == -1) if_name = cptr+i;
		else if_name = cptr+if_pos;

		if_name.trim();
		char* buf = (char*)if_name.data();
		char* pos = StrUtil::strToken(&buf, " ");

		mapMacAddr[buffer] = (pos != NULL) ? pos : "";
	}

#if defined(_WIN32)
	_pclose(in);
#else
	pclose(in);
#endif

	return (int)mapMacAddr.size();	
}

// 커맨드로부터 MAC정보 얻어옴
int GetAllMacAddressByCMD(map<string, string>& mapMacAddr)
{
	mapMacAddr.clear();

	int nic_count = 0;
	const char *cmd = "";
	const char *grep = "";

#if defined(_WIN32)
	_wsetlocale( LC_ALL, L"english" );

	cmd = "ipconfig /all | find \"Physical Address\"";
	grep = "Physical Address";

	nic_count = execute_gec_mac_command(cmd, grep, 0, mapMacAddr);

	_wsetlocale( LC_ALL, L"" );

#elif defined(__linux__) || defined(linux) || defined(__linux)

	cmd = "/sbin/ifconfig";

	if (access(cmd, F_OK) == 0)
	{
		cmd = "/sbin/ifconfig | grep HWaddr";
		grep = "HWaddr ";
		
		nic_count = execute_gec_mac_command(cmd, grep, 0, mapMacAddr);
		
		if (nic_count == 0)
		{
			cmd = "/sbin/ifconfig | grep ether";
			grep = "ether ";
			nic_count = execute_gec_mac_command(cmd, grep, 0, mapMacAddr);	
		}		
	}
	
	if (nic_count == 0)
	{
		cmd = "/sbin/ip addr | grep ether";
		grep = "ether ";
		
		nic_count = execute_gec_mac_command(cmd, grep, 0, mapMacAddr);	
	}
#elif defined(_AIX)
	cmd = "lscfg -vp | grep Network";
	grep = "Network Address";
	
	nic_count = execute_gec_mac_command(cmd, grep, 0, mapMacAddr);
#elif defined(sun) || defined(__sun)
	cmd = "/usr/sbin/arp -a | grep -i `hostname` | awk '{ print $5 }'";
	
	nic_count = execute_gec_mac_command(cmd, grep, mapMacAddr);
#elif defined(_hpux) || defined(hpux) || defined(__hpux)
	cmd = "/usr/sbin/lanscan -a";
	grep = "0x";
	
	nic_count = execute_gec_mac_command(cmd, grep, 0, mapMacAddr);
#else
	cmd = "/sbin/ifconfig -a | grep HWaddr";
	grep = "HWaddr ";
	
	nic_count = execute_gec_mac_command(cmd, grep, 0, mapMacAddr);
#endif

	return nic_count;
}

#if defined(_WIN32)

// 레지스트리에서 값을 읽어옴
BOOL GetVersionValue(LPCTSTR key, LPTSTR version)
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		TCHAR szData[256];
		DWORD dwKeyDataType;
		DWORD dwDataBufSize = 256;
		if (RegQueryValueEx(hKey, key, NULL, &dwKeyDataType, (BYTE*) &szData, &dwDataBufSize) == ERROR_SUCCESS)
		{
			if(dwKeyDataType == REG_SZ)
			{
				ZeroMemory(version, BUFSIZE);
				strcpy_s(version, BUFSIZE-1, szData);
			}
		}
	}

	return TRUE;
}

#endif

__END_NAMESPACE_JFX
