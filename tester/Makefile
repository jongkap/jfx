## Makefile
## 
## Description  : tester Makefile.
## Author       : Jongkap Jeong (jongkap@mail.com)
##
## Copyright (C) Jongkap Jeong, Ltd. All Rights Reserved.
## 
###############################################################################

PRJ_PATH = ..
BIN_PATH = .
INC_PATH = $(PRJ_PATH)/include
LIB_PATH = $(PRJ_PATH)/lib64

PROGS = tester

all: $(PROGS)

CPP = g++

CPPFLAGS = -I$(INC_PATH) -Wall -O2 -fPIC -D_REENTRANT
LIB = -L$(LIB_PATH) -ljfx12 -lpthread 

SRCS =	\
	tester.cpp

OBJS = $(SRCS:.cpp=.o)

.SUFFIXES: .cpp .o

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $<

$(PROGS): $(OBJS)
	$(CPP)  -o $@ $(OBJS) $(LIB)

clean:
	rm -f $(PROGS) *.o

install:
	cp -f $(PROGS) $(BIN_PATH)/$(PROGS)

distclean:
	rm -f $(BIN_PATH)/$(PROGS)

cleanall:
	rm -f $(PROGS) *.o
	rm -f $(BIN_PATH)/$(PROGS)
