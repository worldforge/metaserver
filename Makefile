all: metaserver metaserver.debug

OBJS=metaserver.o wrap.o mainloop.o sock_ntop.o
DBGOBJS=metaserver.dbg wrap.dbg mainloop.dbg sock_ntop.dbg
LIBS=
CFLAGS=-Wall
.SUFFIXES: .cc .o .dbg

.cc.dbg:
	$(CXX) -g -c $(CFLAGS) -DDEBUG -o $@ $<

metaserver: $(OBJS)
	$(CXX) -O2 $(CFLAGS) $(OBJS) $(LIBS) -o $@
	strip $@

metaserver.debug: $(DBGOBJS)
	$(CXX) -g $(CFLAGS) $(DBGOBJS) $(LIBS) -o $@

clean:
	rm -f $(OBJS) $(DBGOBJS) metaserver metaserver.debug
