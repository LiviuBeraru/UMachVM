#ifndef COMMAND_H
#define COMMAND_H

#define OPMAX 256

#include <stdint.h>

enum cmd_format {NUL, NNN, R00, RNN, RR0, RRN, RRR};

struct command {
  uint8_t opcode;
  char   *opname;
  int    (*opfunc) (void);
  enum cmd_format format;
  int    labeled;
};

struct command* command_by_opcode(int opcode);
struct command* command_by_name(const char* name);

#endif
