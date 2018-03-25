#include "execute-cmd.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "shell-context.h"
#include "shell-context/history.h"
#include "commands.h"

static void printErrMsg(EXIT_FLAG exitFlag, ShellCmd const *pCmd);

static int matchCommand(char const *str, ShellCmd cmd) {
  return strcmp(str, cmd.fullname) == 0
    || (cmd.shorthand != NULL && strcmp(str, cmd.shorthand) == 0);
}

// executeCommand: 토큰화된 명령어를 인자로 받아 실제 명령 수행
EXIT_FLAG executeCommand(ShellContextPtr pContext, Arguments args) {
  int i;
  int cmdCnt = getShellCmdCount();
  ShellCmd const *cmdList = getShellCmdList(), *cmdFound = NULL;
  EXIT_FLAG exitFlag = UNKNOWN_COMMAND;
  char const *cmdText = args.argv[0];

  for(i=0; i<cmdCnt; ++i) {
    if(matchCommand(cmdText, cmdList[i])) {
      cmdFound = cmdList + i;
      exitFlag = cmdFound->commandFunc(pContext, args);
      break;
    }
  }

  if(!(exitFlag & EXECUTE_ERROR) && cmdFound->commandFunc != COMMAND_HISTORY) {
    updateHistory(pContext, args.RAW, args.RAW_LEN);
  }
  printErrMsg(exitFlag, cmdFound);
  return exitFlag;
}

static void printErrMsg(EXIT_FLAG exitFlag, ShellCmd const *pCmd) {
  if(exitFlag == 0 || exitFlag & QUIT_SHELL) return;

  printf("-shell: ");
  if(exitFlag & UNKNOWN_COMMAND) {
    printf("unknown command");
  }
  else if(exitFlag & UNKNOWN_ARGUMENT) {
    printf("wrong argument(s)\nusage: %s", pCmd->desc);
  }
  else if(exitFlag & INVALID_ARGUMENT) {
    printf("invalid argument, ");
    if(exitFlag & OUT_OF_RANGE) {
      printf("out of range");
    }
    else if(exitFlag & NOT_HEX) {
      printf("non-hexadecimal");
    }
    else if(exitFlag & ARGUMENT_TOO_LONG) {
      printf("argument too long");
    }
    else {
      printf("<unknown>");
    }
  }
  else {
    printf("<unknown error>");
  }
  putchar('\n');
}