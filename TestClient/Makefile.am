
CFLAGS=-Wall -I/usr/local/include -I../ -g -DDEBUG
CXXFLAGS=-Wall -I/usr/local/include -I../
#LIBS = -L/usr/local/lib -lgnugetopt

bin_PROGRAMS = testclient #testclient.debug

noinst_HEADERS =\
 wrap.h dgclibig.h

testclient_SOURCES =\
 testclient.c wrap.c sock_ntop.c dgclibig.c
