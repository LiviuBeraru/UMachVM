CC = gcc
PROG = uasm

OBJ = uasm.o asm_data.o asm_labels.o asm_formats.o\
      command.o registers.o strings.o

CFLAGS = -Wall -ggdb

$(PROG): $(OBJ)
	$(CC) -o $(PROG) $(OBJ)


%.o: %.c 
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm *.o
