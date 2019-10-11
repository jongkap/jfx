// $Id: $
/*******************************************************************************
 * Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
 *
 * file:    jfxtest.cpp
 * author:  kbroad@mail.com
 * date:    2008/07/10 - first relase
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if !defined(_WIN32)
  #include <unistd.h>
  #include <pthread.h>
#endif

#include "jfx.h"
using namespace jfx;

#define TEST_CONF		"normal.conf"
#define MAX_THREADS		200

int logger_tester();
int config_tester();
int bstring_tester();
int jstring_tester();
int JSocket_tester();
int sysutil_tester();
int strutil_tester();

/// @fn int main(int argc, char** argv)
/// @brief	tester Main 함수
/// @param	argc	[in] 인자 개수
/// @param	argv	[in] 인자
/// @return 성공 여부
int main(int argc, char** argv)
{
	logger_tester();
	config_tester();
	bstring_tester();
	sysutil_tester();
	strutil_tester();
	return 0;
}

void* make_log_thread(void* param)
{
	int err = 0;
	int thr_no = *(int *)param;

	for (int i = 0; i < 1000000; i++)
	{
		err = JfxLog(LV_INFO, "thread log[%d]: %d\n", thr_no, i);
		if (err < 0) 
		{
			fprintf(stdout, "thread log[%d]: error: %s\n", thr_no, Logger.getLastError());
			break;
		}
	}


	return NULL;
}

int logger_tester()
{
#if defined(_WIN32)
	HANDLE hThread[MAX_THREADS];
#else
	pthread_t hThread[MAX_THREADS];
#endif

	clock_t before;
	double result;

	before = clock();

	fprintf(stdout, "[Logger]\n");
	JfxLogger("test.log", "trace");

	fprintf(stdout, "[1] Init ok: file(%s), mode(%s)\n", "test.log", "trace");

	fprintf(stdout, "[2] Info log... \n");
	for (int i =0; i < 1000; i++)
		JfxLog(LV_INFO, "test log: %d\n", i+1);


	fprintf(stdout, "[3] Debug log...\n");
	for (int i =0; i < 10; i++)
		JfxDebug("test debug log: %d\n", i+1);

	fprintf(stdout, "[4] test trace log");
	JfxTrace("[4] test trace log\n");

	bstring dump;
	dump.push(4);
	dump.push(4);
	dump.push(5);
	dump.push(4);

	JfxDump("dump....\n    ", dump.data(), dump.size());
	fprintf(stdout, "\n");

	result = (double)(clock() - before) / CLOCKS_PER_SEC;
	fprintf(stdout, "elapsed time: %5.2f.\n", result);

	fprintf(stdout, "[5] thread test: count(%d)\n", MAX_THREADS);

	before = clock();

	for(int i=0; i<MAX_THREADS; i++)
    {
		int thr_no = i;
		fprintf(stdout, "---> create thread: %d\n", thr_no);

#if defined(_WIN32)
		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)make_log_thread, (void *)&thr_no, 0, NULL);
		if (hThread[i] == NULL)
		{
			fprintf(stdout, "error: %s\n", StrUtil::strError(GetLastError()));
			break;
		}
#else
		int err = pthread_create(&hThread[i], NULL, make_log_thread, (void *)&thr_no);
		if (err != 0)
		{
			fprintf(stdout, "error: %s\n", StrUtil::strError(err));
			break;
		}
#endif
	}

    // Close all thread handles upon completion.
    for(int i=0; i<MAX_THREADS; i++)
    {
#if defined(_WIN32)
		WaitForSingleObject(hThread[i],INFINITE);
		CloseHandle(hThread[i]);
		hThread[i] = 0;
#else
		pthread_join(hThread[i], NULL);
		hThread[i] = 0;
#endif
    }

	result = (double)(clock() - before) / CLOCKS_PER_SEC;
	fprintf(stdout, "elapsed time: %5.2f.\n", result);

	return 0;
}

int config_tester()
{
	fprintf(stdout, "[IniFile]\n");

	//IniFile config(TEST_CONF);
	//if (config.getLastError() < 0) 
	//{
	//	printf("Can't load '%s'\n\n", TEST_CONF);
	//	return 1;	
	//}
	
	IniFile config;
	config.loadFile(TEST_CONF);
	
	fprintf(stdout, "[1] Config loaded from '%s': \n", TEST_CONF);
	fprintf(stdout, "[2] version=%d, name=%s, email=%s, pi=%f, active=%d\n",
			config.getInteger("protocol", "version", -1),
			config.getString("user", "name", "UNKNOWN").c_str(),
			config.getString("user", "email", "UNKNOWN").c_str(),
			config.getReal("user", "pi", -1),
			config.getBoolean("user", "active", true)
			);
	fprintf(stdout, "\n");
			
	return 0;
}

int bstring_tester()
{
	fprintf(stdout, "[bstring]\n");
	
	bstring bstr;	
	bstr.format("[%d] - name: %s", 5, "tester");
	
	fprintf(stdout, "[1] format: %s\n", bstr.c_str());

	bstr.free();
	bstr.malloc(1024);
	
	memcpy(bstr.dataptr(), "1234567890", 10);

	*bstr.sizeptr() = 10;
	fprintf(stdout, "[2] malloc: %s (%u)\n", bstr.c_str(), bstr.size());

	bstr.realloc(1025);
	fprintf(stdout, "[3] realloc: max_size(%d)\n", bstr.max_size());
	
	bstr.clear();
	for (int key = 0; key < 16; key++) bstr.push(key+0x11);
	
	string out;
	StrUtil::hexDump(&out, NULL, bstr.data(), bstr.size());
	
	fprintf(stdout, "[4] push (16bytes)\n");
	fprintf(stdout, "%s\n", out.c_str());
	
	bstr.append(bstr.data(), 8);
	StrUtil::hexDump(&out, NULL, bstr.data(), bstr.size());
	
	fprintf(stdout, "[5] append (8bytes)\n");
	fprintf(stdout, "%s\n", out.c_str());
	
	
	fprintf(stdout, "\n");
	return 0;
}

int jstring_tester()
{
	fprintf(stdout, "[jstring]\n");


	jstring str;
	str.format("%s, %d", "abcDxfF", 5);

	fprintf(stdout, "[1] format: |%s|\n", str.c_str());
	fprintf(stdout, "[2] upper case: |%s|\n", str.toupper().c_str());
	fprintf(stdout, "[3] lower case: |%s|\n", str.tolower().c_str());

	str.format(" \t abc   ");
	fprintf(stdout, "[4] rtrim: |%s|", str.c_str());
	fprintf(stdout, " -> |%s|\n", str.rtrim().c_str());

	fprintf(stdout, "[5] ltrim: |%s|", str.c_str());
	fprintf(stdout, " -> |%s|\n", str.ltrim().c_str());

	str.format(" \t abc   ");
	fprintf(stdout, "[6] trim: |%s|", str.c_str());
	fprintf(stdout, " -> |%s|\n", str.trim().c_str());


	str.format("conf = ${path}/name.conf");
	fprintf(stdout, "[7] replace: |%s|", str.c_str());
	fprintf(stdout, " -> |%s|\n", str.replace("${path}", "/home/kbroad").c_str());

	str = "12";
	fprintf(stdout, "[8] strtoi: |%s| -> |%d|\n", str.c_str(), str.strtoi());
	str = "12.34";
	fprintf(stdout, "[9] strtof: |%s| -> |%f|\n", str.c_str(), str.strtof());
	str = "12345678901234.987654";
	fprintf(stdout, "[10] strtod: |%s| -> |%.5f|\n", str.c_str(), str.strtod());
	str = "123456789";
	fprintf(stdout, "[11] strtol: |%s| -> |%ld|\n", str.c_str(), str.strtol());
	str = "123456789";
	fprintf(stdout, "[12] strtoul: |%s| -> |%lu|\n", str.c_str(), str.strtoul());

	str = "abcd12";
	jstring s2 = "abCd12";
	fprintf(stdout, "[13] compare: |%s| == |%s|", str.c_str(), s2.c_str());
	fprintf(stdout, " -> |%d|\n", str.compare(s2));

	fprintf(stdout, "[14] compare: |%s| == |%s|, count: %d", str.c_str(), s2.c_str(), 2);
	fprintf(stdout, " -> |%d|\n", str.compare(s2, 2));
	
	fprintf(stdout, "[15] casecompare: |%s| == |%s|", str.c_str(), s2.c_str());
	fprintf(stdout, " -> |%d|\n", str.casecompare(s2));

	str = "abcd123";
	s2 = "abCd12";
	fprintf(stdout, "[16] casecompare: |%s| == |%s|, count: %d", str.c_str(), s2.c_str(), 6);
	fprintf(stdout, " -> |%d|\n", str.casecompare(s2, 6));

	fprintf(stdout, "[17] add_equal(+=): |%s| += |%s|", str.c_str(), s2.c_str());
	str += s2;
	fprintf(stdout, " -> |%s|\n", str.c_str());

	fprintf(stdout, "[18] addl(+): |%s| = |%s| + |%s|", str.c_str(), s2.c_str(), "_qwerty");
	str = s2 + "_qwerty";
	fprintf(stdout, " -> |%s|\n", str.c_str());

	
	fprintf(stdout, "[19] operation(==): |%s| == |%s|, result(%d)\n", str.c_str(), s2.c_str(), str == s2);

	fprintf(stdout, "\n");
	return 0;
}

int JSocket_tester()
{
	fprintf(stdout, "[JSocket]\n");

	const char* ip = "192.168.50.76";
	int port = 8320;

	JSocket sock;

	fprintf(stdout, "try to: %s:%d... ", ip, port);

	int nsock = sock.nbconnect(ip, port, 0, 5);

	fprintf(stdout, "socket(%d), reason(%s)\n", nsock, sock.getLastErrorString());

	fprintf(stdout, "\n");
	return 0;	
}


int sysutil_tester()
{
	string os_platform;
	string os_username;
	string mac_addr;
	string proc_name;
	string current_dir;
/*
	// OS 플랫폼 정보 얻기
	SysUtil::getOSPlatform(os_platform);
	
	// OS 사용자명 얻기
	SysUtil::getOSUserName(os_username); 
	
	// MAC Addesss 얻기
	SysUtil::getMacAddress(mac_addr);

	// 실행 프로세스 얻기
	SysUtil::getProcessName(proc_name);
	
	// 현재 디렉토리 얻기
	SysUtil::getCurrentDir(current_dir);


	fprintf(stdout, "[SysUtil]\n");
	fprintf(stdout, "os_platform = %s\n", os_platform.c_str());
	fprintf(stdout, "os_username = %s\n", os_username.c_str());
	fprintf(stdout, "mac_addr = %s\n", mac_addr.c_str());
	fprintf(stdout, "proc_name = %s\n", proc_name.c_str());
	fprintf(stdout, "current_dir = %s\n", current_dir.c_str());
	fprintf(stdout, "\n");
*/
	return 0;
}


int strutil_tester()
{
	/*
	fprintf(stdout, "[StrUtil]\n");
	fprintf(stdout, "toString: %s\n", StrUtil::toString(5).c_str());

	fprintf(stdout, "upperString: %s\n", StrUtil::upperString("abcdEFgH").c_str());
	fprintf(stdout, "lowerString: %s\n", StrUtil::lowerString("abcdEFgH").c_str());

	string str;
	StrUtil::formatString(str, "%d, %s", time(NULL), "test");
	fprintf(stdout, "format string: %s\n", str.c_str());
	
	// base64 인코딩
	string in_str = "1234567890!@#";
	string hash_str;
	StrUtil::base64Encode(hash_str, (byte_t*)in_str.c_str(), in_str.size());
	
	// base64 디코딩
	byte_t out_byte[1024] = {0, };
	size_t out_len;
	StrUtil::base64Decode(out_byte, out_len, hash_str);

	fprintf(stdout, "base64Encoding: %s\n", hash_str.c_str());
	fprintf(stdout, "base64Decoding: %s\n", (char*)out_byte);
	*/
	
	return 0;
}
