all: testclient testclient.debug show-result

OBJS=testclient.o wrap.o dgclibig.o sock_ntop.o 
DBGOBJS=testclient.dbg wrap.dbg dgclibig.dbg sock_ntop.dbg
LIBS=
CFLAGS=-Wall -I../
.SUFFIXES: .c .o .dbg

.c.dbg:
	$(CC) -g -c $(CFLAGS) -o $@ $<

testclient: $(OBJS)
	$(CC) -O2 $(CFLAGS) $(OBJS) $(LIBS) -o $@
	strip $@

testclient.debug: $(DBGOBJS)
	$(CC) -g $(CFLAGS) $(DBGOBJS) $(LIBS) -o $@

show-result:
	ls -l

clean:
	rm -f $(OBJS) $(DBGOBJS) testclient testclient.debug
