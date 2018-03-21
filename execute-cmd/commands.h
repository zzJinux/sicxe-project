#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "typedefs.h"

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