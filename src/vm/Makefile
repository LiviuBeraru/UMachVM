CC = gcc
PROG = umach

OBJ = umach.o core.o memory.o registers.o \
      controll.o loadstore.o arithm.o logic.c compare.o branch.o subroutine.o\
      system.o io.o\
      logmsg.o command.o disassemble.o debugger.o\
      assemble.o asm_labels.o\
      strings.o

#CFLAGS = -Wall -gdwarf-2 -g3
CFLAGS = -Wall -ggdb

$(PROG): $(OBJ)
	$(CC) -o $(PROG) $(OBJ)


%.o: %.c 
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm *.o
