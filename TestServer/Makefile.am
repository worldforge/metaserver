
CFLAGS=-Wall -I/usr/local/include -I../ -g -DDEBUG
CXXFLAGS=-Wall -I/usr/local/include -I../
#LIBS = -L/usr/local/lib -lgnugetopt

bin_PROGRAMS = testserver #testserver.debug

noinst_HEADERS =\
 wrap.h dgclibig.h

testserver_SOURCES =\
 testserver.c wrap.c sock_ntop.c dgclibig.c

