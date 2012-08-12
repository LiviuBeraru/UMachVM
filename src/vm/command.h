#ifndef COMMAND_H
#define COMMAND_H

#define OPMAX 256

enum cmd_format {NUL, NNN, R00, RNN, RR0, RRN, RRR};

struct command {
  int     opcode;
  char   *opname;
  int    (*opfunc) (void);
  enum cmd_format format;
};

struct command* command_by_opcode(int opcode);

#endif
