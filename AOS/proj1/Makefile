#### GTThread Library Makefile

CFLAGS  = -Wall -pedantic -w
LFLAGS  =
CC      = gcc
RM      = /bin/rm -rf
AR      = ar rc
RANLIB  = ranlib

LIBRARY = gtthread.a

LIB_SRC = gtthread.c gtthread_mutex.c #gtthread_sched.c

LIB_OBJ = $(patsubst %.c,%.o,$(LIB_SRC))

PROG_SRC = DiningPhilosophers.c
PROG_OBJ = DiningPhilosophers

# pattern rule for object files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(LIBRARY) $(PROG_OBJ)

$(LIBRARY): $(LIB_OBJ)
	$(AR) $(LIBRARY) $(LIB_OBJ)
	$(RANLIB) $(LIBRARY)

clean:
	$(RM) $(LIBRARY) $(LIB_OBJ) $(PROG_OBJ)

$(PROG_OBJ): $(LIBRARY)
	$(CC) $(CFLAGS) $(PROG_SRC) $(LIBRARY) -o $(PROG_OBJ) -I.

.PHONY: depend
depend:
	$(CFLAGS) -- $(LIB_SRC)  2>/dev/null
