#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "typedefs.h"
#include "shell-context.h"

typedef struct _Arguments {
  char const *RAW; int RAW_LEN;
  int argc; char **argv;
} Arguments;

typedef enum _EXIT_FLAG {
  QUIT_SHELL = 1<<0,
  UNKNOWN_COMMAND = 1<<1,
  UNKNOWN_ARGUMENT = 1<<2,
  OUT_OF_RANGE = 1<<3,
  NOT_HEX = 1<<4,
  ARGUMENT_TOO_LONG = 1<<5,
  INVALID_ARGUMENT = OUT_OF_RANGE | NOT_HEX | ARGUMENT_TOO_LONG,
  EXECUTE_ERROR =  UNKNOWN_COMMAND | UNKNOWN_ARGUMENT | INVALID_ARGUMENT
} EXIT_FLAG;

typedef EXIT_FLAG (*COMMAND_FUNC)(ShellContextPtr, const Arguments);

typedef struct _ShellCmd {
  char const *fullname, *shorthand, *desc;
  COMMAND_FUNC commandFunc;
} ShellCmd;

int getShellCmdCount(void);
ShellCmd const *getShellCmdList(void);
EXIT_FLAG COMMAND_HELP(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_DIR(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_QUIT(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_HISTORY(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_DUMP(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_EDIT(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_FILL(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_RESET(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_OPCODE(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_OPCODELIST(ShellContextPtr, const Arguments);

#endif