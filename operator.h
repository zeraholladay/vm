#ifndef OPERATOR_H
#define OPERATOR_H

struct op {
  int op_code;
  int nargs;
};

const struct op *lookup_op(register const char *str, register unsigned int len);
#endif