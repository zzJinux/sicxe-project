#include <stdio.h>
#include <string.h>
#include "shell-context.h"
#include "execute-cmd.h"
#define MAX_LEN 256

void flushRestInput(char *temp);

int main() {
  ShellContextPtr pContext = initShellContext();
  if(pContext == NULL) {
    printf("- error: dynamic memory allocation failed\n");
  }

  while(1) {
    printf("sicsim> ");
    // stdin으로부터 최대 MAX_LEN 길이의 커맨드 입력받음 (+2는 newline과 null)
    char input[MAX_LEN+2];
    fgets(input, MAX_LEN+2, stdin); // cmd는 null-termination이 보장됨
    int len = strlen(input);

    Arguments args;
    unsigned errorCode;
    errorCode = parseInput(&args, input, len);

    if(errorCode & PARSE_FAIL) {
      errorCode &= ~PARSE_FAIL;
      if(errorCode & INVALID_FORMAT) {
        errorCode &= ~INVALID_FORMAT;
        char *desc = "";
        if(errorCode & NOT_LOWERCASE) {
          desc = "only lowercase alphabets are allowed for command";
        }
        else if(errorCode & INVALID_DELIMITER) {
          desc = "invalid delimeter between arguments";
        }
        printf("- error: invalid command format, %s", desc);
      }
      else if(errorCode & ALLOCATION_ERROR) {
        printf("- error: dynamic memory allocation failed");
      }
      else {
        printf("- error: unknown error");
      }
      putchar('\n');
      continue;
    }
    else {
      if(errorCode & COMMAND_EMPTY) {
        continue;
      }
    }

    args.RAW = input;
    args.RAW_LEN = len;

    EXIT_FLAG exitFlag = executeCommand(pContext, args);
    deallocArguments(args);

    if(exitFlag & EXECUTE_ERROR) {
      exitFlag &= ~EXECUTE_ERROR;
      if(exitFlag & UNKNOWN_COMMAND) {
        printf("- error: unknown command");
      }
      else if(exitFlag & UNKNOWN_ARGUMENT) {
        printf("- error: unknown argument");
      }
      putchar('\n');
    }
    if(exitFlag == QUIT_SHELL) {
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
