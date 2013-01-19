
# makefile for EPOS lib 

CC = gcc
CPP = g++

CFLAGS = -Wall -O2  #-DDEBUG -DDDEBUG  -ggdb

INC = 

COMPILE = $(CC) $(CFLAGS) $(INC) -c
COMPILE++ = $(CPP) $(CFLAGS) $(INC) -c


# Linker
LD = $(CC)

# Die Linker Flags
LDFLAGS = -L. -static

# Bibliotheken, die wir linken wollen
LIBS = -lepos #-lefence




#---------------------
all: libepos.a libepos++.a epos_sample

PROGRAM = filterwheel
OBJS =   filterwheel.o

$(PROGRAM): $(OBJS) libepos.a
	$(LD) -o $(PROGRAM) $(LDFLAGS) $(OBJS) $(LIBS) 

# $? ist alles nach dem ':' in der Zielliste, $< nur das erste
# $@ ist das Ziel


epos.o: epos.c epos.h
	$(COMPILE) -o $@ $<


epos.opp: epos.c epos.h
	$(COMPILE++) -o $@ $<


libepos.a: epos.o
	ar crs $@ $?
	ranlib $@

libepos++.a: epos.opp 
	ar crs $@ $?
	ranlib $@



filterwheel.o: filterwheel.c epos.h
	$(COMPILE) -o $@ $<



epos_sample.o: epos_sample.c epos.h
	$(COMPILE) -o $@ $<

epos_sample: epos_sample.o libepos.a
	$(LD) -o $@ $(LDFLAGS) $< $(LIBS) 




epos_off.o: epos_off.c epos.h epos.c
	$(COMPILE) -o $@ $<
epos_off: epos_off.o epos.o
	$(LD) -o $@ $(LDFLAGS) epos_off.o epos.o $(LIBS) 


ttt.o: ttt.c epos.h epos.c
	$(COMPILE) -o $@ $<
ttt:  ttt.o epos.o
	$(LD) -o $@ $(LDFLAGS) ttt.o epos.o $(LIBS) 



.PHONY: clean compile tar

clean:
	rm -f $(PROGRAM) libepos* *.o *.opp epos_sample ttt

compile: $(OBJS)

tar:
	tar cvzhf libepos.tar.gz README makefile epos.c epos.h Doxyfile refman.pdf epos_sample.c