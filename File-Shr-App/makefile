UNAME = $(shell uname)
ifeq ($(UNAME), SunOS) # Sun OS
MY_LIBS = -lresolv -lsocket -lnsl
endif
ifeq ($(UNAME), Linux) # Linux
MY_LIBS = -lresolv -lnsl -lpthread
endif
ifeq ($(UNAME), Darwin) # Mac OS
MY_LIBS =
endif

CC:=/usr/bin/g++

all:
	${CC} fileShrApp.cpp -o fileShrApp

##==========================================================================
clean:;@- $(RM) fileShrApp
	@- echo “Data Cleansing Done.Ready to Compile”
