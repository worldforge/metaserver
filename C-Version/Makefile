all: metaserver metaserver.debug show-result

OBJS=metaserver.o wrap.o dgecholoop.o
DBGOBJS=metaserver.dbg wrap.dbg dgecholoop.dbg
LIBS=
CFLAGS=-Wall
.SUFFIXES: .c .o .dbg

.c.dbg:
	$(CC) -g -c $(CFLAGS) -o $@ $<

metaserver: $(OBJS)
	$(CC) -O2 $(CFLAGS) $(OBJS) $(LIBS) -o $@
	strip $@

metaserver.debug: $(DBGOBJS)
	$(CC) -g $(CFLAGS) $(DBGOBJS) $(LIBS) -o $@

show-result:
	ls -l

clean:
	rm -f $(OBJS) $(DBGOBJS) metaserver metaserver.debug
