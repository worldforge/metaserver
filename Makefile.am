
SUBDIRS = TestClient TestServer
EXTRA_DIST = TODO

CFLAGS=-Wall -I/usr/local/include
CXXFLAGS=-Wall -I/usr/local/include
#LIBS = -L/usr/local/lib -lgnugetopt

bin_PROGRAMS = metaserver #metaserver.debug

noinst_HEADERS =\
 wrap.hh mainloop.hh protocol_instructions.h

metaserver_SOURCES =\
 metaserver.cc wrap.cc mainloop.cc sock_ntop.cc

