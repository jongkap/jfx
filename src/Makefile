## Makefile
## 
## Description  : jfx lib Makefile.
## Author       : Jongkap Jeong (jongkap@mail.com)
##
## Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
## 
###############################################################################

VERSION = 12
PRJ_PATH = ..
INC_PATH = $(PRJ_PATH)/include
LIB_PATH = $(PRJ_PATH)/lib64

PROGS = jfx$(VERSION)

all: $(PROGS)

CPP = g++
SLIB_EXT = a

CPPFLAGS = -I$(INC_PATH) -Wall -O2 -fPIC -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE

SRCS =	\
	bstring.cpp \
	IniFile.cpp \
	jerror.cpp \
	jmutex.cpp \
	JSocket.cpp \
	jstring.cpp \
	Logger.cpp \
	StrUtil.cpp \
	SysUtil.cpp

OBJS = $(SRCS:.cpp=.o)

.SUFFIXES: .cpp .o

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $<

$(PROGS): $(OBJS)
	ar  rcv lib$@.$(SLIB_EXT) $(OBJS)
	ranlib lib$@.$(SLIB_EXT)

clean:
	rm -f lib$(PROGS).$(SLIB_EXT) *.o

install:
	cp -f jfx.h $(INC_PATH)
	cp -f lib$(PROGS).$(SLIB_EXT) $(LIB_PATH)/lib$(PROGS).$(SLIB_EXT)

distclean:
	rm -f $(INC_PATH)/*.h
	rm -f $(LIB_PATH)/lib$(PROGS).$(SLIB_EXT)

cleanall:
	rm -f lib$(PROGS).$(SLIB_EXT) *.o
	rm -f $(INC_PATH)/*.h
	rm -f $(LIB_PATH)/lib$(PROGS).$(SLIB_EXT)
