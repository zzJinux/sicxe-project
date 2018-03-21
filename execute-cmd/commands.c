#include "commands.h"

#include <stdio.h>
#include "history.h"
#include "execute-cmd.h"

int const SHELL_COMMANDS_CNT = 10;
ShellCmd const SHELL_COMMANDS[SHELL_COMMANDS_CNT] = {
  {"help", "h", "h[elp]", COMMAND_HELP},
  {"dir", "d", "d[ir]", COMMAND_DIR},
  {"quit", "q", "q[uit]", COMMAND_QUIT},
  {"history", "hi", "hi[story]", COMMAND_HISTORY},
  {"dump", "dm", "du[mp] [start, end]", COMMAND_DUMP},
  {"edit", "e", "e[dit] address, value", COMMAND_EDIT},
  {"fill", "f", "f[ill] start, end, value", COMMAND_FILL},
  {"reset", NULL, "reset", COMMAND_RESET},
  {"opcode", NULL, "opcode mnemoic", COMMAND_OPCODE},
  {"opcodelist", NULL, "opcodelist", COMMAND_OPCODELIST}
};

EXIT_FLAG COMMAND_HELP(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }
  updateHistory(pContext, args.RAW, args.RAW_LEN);

  int i;
  for(i=0; i<SHELL_COMMANDS_CNT; ++i) {
    printf("%s\n", SHELL_COMMANDS[i].desc);
  }

  return 0;
}

EXIT_FLAG COMMAND_DIR(ShellContextPtr pContext, const Arguments args) {
  // TODO
  return 0;
}

// EXIT_FLAG COMMAND_DIR(ShellContextPtr, const Arguments) {}
EXIT_FLAG COMMAND_QUIT(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }
  updateHistory(pContext, args.RAW, args.RAW_LEN);
  return QUIT_SHELL;
}

EXIT_FLAG COMMAND_HISTORY(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }
  updateHistory(pContext, args.RAW, args.RAW_LEN);
  printHistory(pContext);
  return 0;
}
EXIT_FLAG COMMAND_DUMP(ShellContextPtr pContext, const Arguments args) {
  // TODO
  return 0;
}
EXIT_FLAG COMMAND_EDIT(ShellContextPtr pContext, const Arguments args) {
  // TODO
  return 0;
}
EXIT_FLAG COMMAND_FILL(ShellContextPtr pContext, const Arguments args) {
  // TODO
  return 0;
}
EXIT_FLAG COMMAND_RESET(ShellContextPtr pContext, const Arguments args) {
  // TODO
  return 0;
}
EXIT_FLAG COMMAND_OPCODE(ShellContextPtr pContext, const Arguments args) {
  // TODO
  return 0;
} 
EXIT_FLAG COMMAND_OPCODELIST(ShellContextPtr pContext, const Arguments args) {
  // TODO
  return 0;
}