#ifndef _ASSEMBLE_ERRS_H_
#define _ASSEMBLE_ERRS_H_

typedef enum _ASSEMBLE_ERROR {
  ADDR_RESOLUTION_FAIL = 1<<3,
  SYNTAX_PARSE_FAIL = 1<<4,
  ALLOC_FAIL = 1<<5
} ASSEMBLE_ERROR;

typedef enum _SYNTAX_ERROR {
  START_DIRECTIVE_WRONG_PLACE = 1,
  PROGRAM_NAME_TOO_LONG,
  LOC_EXCEEDED,
  INVALID_OPERAND_FORMAT,
  INVALID_SYMBOL_NAME,
  OPERAND_MISSING,
  DUPLICATE_SYMBOL,
  UNDEFINED_OPCODE,
  UNDEFINED_SYMBOL,
  CONSTANT_TOO_LARGE,
  TOKEN_MISSING,
  UNDEFINED_REGISTER
} SYNTAX_ERROR;

char const *SYNTAX_ERROR_MESSAGES[] = {
  "<no-error>",
  "START directive at wrong place",
  "PROGRAM_NAME too long",
  "location counter exceeded the limit",
  "invalid operand format",
  "invalid symbol name",
  "operand is missing",
  "symbol is already defined",
  "opcode undefined",
  "symbol undefined",
  "constant is out-of-range",
  "token is missing",
  "register undefined"
};

#endif