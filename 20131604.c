#include <stdio.h>
#include <string.h>
#include "shell-context.h"
#include "execute-cmd.h"
#define MAX_LEN 256

void flushRestInput(char *temp);

int main() {
  FILE *opcodeRaw;
  opcodeRaw = fopen("opcode.txt", "r");
  if(opcodeRaw == NULL) {
    printf("- error: file open failed\n");
    return -1;
  }

  ShellContextPtr pContext = initShellContext(opcodeRaw);
  fclose(opcodeRaw);
  if(pContext == NULL) {
    printf("- error: memory allocation failed\n");
    return -1;
  }

  while(1) {
    printf("sicsim> ");
    // stdin으로부터 최대 MAX_LEN 길이의 커맨드 입력받음 (+2는 newline과 null)
    char input[MAX_LEN+2];
    fgets(input, MAX_LEN+2, stdin); // cmd는 null-termination이 보장됨
    int len = strlen(input);

    Arguments args;
    PARSE_RESULT errCode = parseInput(&args, input, len);
    if(errCode & PARSE_ERROR || errCode & COMMAND_EMPTY) {
      continue;
    }

    args.RAW = input;
    args.RAW_LEN = len;

    EXIT_FLAG exitFlag = executeCommand(pContext, args);
    deallocArguments(args);

    if(exitFlag & QUIT_SHELL) {
      puts("Exit sicsim...");
      break;
    }

    flushRestInput(input);
  }

  cleanupShellContext(pContext);
  return 0;
}

void flushRestInput(char *last) {
  int len = strlen(last);
  while(last[len-1] != '\n') {
    fgets(last, len+1, stdin);
    len = strlen(last);
  }
}
