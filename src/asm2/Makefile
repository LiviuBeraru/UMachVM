CC = gcc
PROG = uasm

OBJ = asm_formats.o assemble.o collect_data.o commands.o \
      registers.o str_func.o symbols.o uasm.o

CFLAGS = -std=gnu99 -Wall -pipe -g `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`

$(PROG): $(OBJ)
	$(CC) -o $(PROG) $(OBJ) $(LDFLAGS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm *.o
