all: testserver testserver.debug show-result


OBJS=testserver.o wrap.o dgclibig.o sock_ntop.o 
DBGOBJS=testserver.dbg wrap.dbg dgclibig.dbg sock_ntop.dbg
LIBS=
CFLAGS=-Wall -I../
.SUFFIXES: .c .o .dbg

.c.dbg:
	$(CC) -g -c $(CFLAGS) -o $@ $<

testserver: $(OBJS)
	$(CC) -O2 $(CFLAGS) $(OBJS) $(LIBS) -o $@
	strip $@

testserver.debug: $(DBGOBJS)
	$(CC) -g $(CFLAGS) $(DBGOBJS) $(LIBS) -o $@

show-result:
	ls -l

clean:
	rm -f $(OBJS) $(DBGOBJS) testserver testserver.debug
