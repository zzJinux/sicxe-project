#include "execute-cmd.h"

#include <stddef.h>
#include <string.h>
#include "commands.h"

extern int const SHELL_COMMANDS_CNT;
extern ShellCmd const SHELL_COMMANDS[];

int matchCommand(char const *str, ShellCmd cmd) {
  return strcmp(str, cmd.fullname) == 0
    || (cmd.shorthand != NULL && strcmp(str, cmd.shorthand) == 0);
}

// executeCommand: 토큰화된 명령어를 인자로 받아 실제 명령 수행
EXIT_FLAG executeCommand(ShellContextPtr pContext, Arguments args) {
  int i;
  char const *cmdText = args.argv[0];
  for(i=0; i<SHELL_COMMANDS_CNT; ++i) {
    if(matchCommand(cmdText, SHELL_COMMANDS[i])) {
      return SHELL_COMMANDS[i].commandFunc(pContext, args);
    }
  }
  return UNKNOWN_COMMAND;
}
