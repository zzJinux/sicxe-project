#ifndef _EXECUTE_CMD_H_
#define _EXECUTE_CMD_H_

#include "typedefs.h"
#include "commands.h"

typedef enum _PARSE_RESULT {
  COMMAND_EMPTY = 1<<0,
  NOT_LOWERCASE = 1<<1,
  INVALID_DELIMITER = 1<<2,
  ALLOCATION_ERROR = 1<<3,
  INVALID_FORMAT = NOT_LOWERCASE | INVALID_DELIMITER,
  PARSE_ERROR = INVALID_FORMAT | ALLOCATION_ERROR
} PARSE_RESULT;

// parseCommand: 명령어 텍스트를 토큰들의 배열로 파싱
PARSE_RESULT parseInput(Arguments *pArgs, char *rawCmd, int cmdLength);

// deallocCommand: parseCommand 에서 수행한 메모리 반환
void deallocArguments(Arguments args);

EXIT_FLAG executeCommand(ShellContextPtr pContext, Arguments args);

#endif
