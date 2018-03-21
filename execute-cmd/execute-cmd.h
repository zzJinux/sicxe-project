#ifndef _EXECUTE_CMD_H_
#define _EXECUTE_CMD_H_

/*
  parseCommand Flags:

  00000 : NO ERROR
  00010 : COMMAND EMPTY
  00001 : ERROR FOUND
  00011 : INVALID FORMAT
  00111 : NOT LOWERCASE
  01011 : INVALID DELIMITER
  10001 : ALLOCATION ERROR
*/
#define COMMAND_EMPTY 2
#define ERROR_FOUND 1
#define INVALID_FORMAT 3
#define NOT_LOWERCASE 7
#define INVALID_DELIMITER 11
#define ALLOCATION_ERROR 17

#define EXIT_FLAG 16

typedef struct _Arguments {
  int argc; char **argv;
} Arguments;

// parseCommand: 명령어 텍스트를 토큰들의 배열로 파싱
unsigned parseInput(Arguments *pArgs, char *rawCmd, int cmdLength);

// deallocCommand: parseCommand 에서 수행한 메모리 반환
void deallocArguments(Arguments args);

unsigned executeCommand(Arguments args);

#endif
