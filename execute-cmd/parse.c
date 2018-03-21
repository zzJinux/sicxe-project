#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "execute-cmd.h"

/* internal functions */
int saveToBuffer(char *buffer, int pos, char *src, int len);
char jumpBlank(char *str, int *pPos);

// parseCommand: 명령어 텍스트를 토큰들의 배열로 파싱
unsigned parseInput(Arguments *pArgs, char *rawCmd, int cmdLength) {
  unsigned errorCode = 0;
  int i=0;
  char ch;

  ch = jumpBlank(rawCmd, &i);
  if(ch == '\0') {
    return COMMAND_EMPTY;
  }

  char **argv = NULL;
  char *buffer = NULL;

  int argcReserve = 6;
  int argc = 0;
  argv = (char **)malloc(argcReserve * sizeof(char *));
  if(argv == NULL) {
    errorCode = ALLOCATION_ERROR;
    goto cleanup;
  }

  int bufferPos = 0;
  // 파싱 후 문자열들의 길이의 총합은 raw input 보다 반드시 크지 못함
  buffer = (char *)malloc((cmdLength+1) * sizeof(char));
  if(buffer == NULL) {
    errorCode = ALLOCATION_ERROR;
    goto cleanup;
  }

  // 본 명령어, 즉, 0번째 인자의 경계를 찾는다
  // 소문자가 아닌 문자가 나오면 에러코드를 리턴
  int start = i;
  for(; !isspace(ch=rawCmd[i]) && ch != '\0'; ++i) {
    if(ch < 'a' || ch > 'z') {
      errorCode = NOT_LOWERCASE;
      goto cleanup;
    }
  }

  int argLen = i - start;
  int nextPos = saveToBuffer(buffer, bufferPos, rawCmd + start, argLen);

  // NOTE: extract to function?
  if(argc+1 > argcReserve) {
    argv = realloc(argv, argcReserve += 6);
  }
  argv[argc++] = buffer + bufferPos;
  bufferPos = nextPos;
  
  if(ch == '\0') {
    goto parse_end;
  }

  ch = jumpBlank(rawCmd, &i);
  if(ch == '\0') {
    goto parse_end;
  }
  if(ch == ',') {
    errorCode = INVALID_DELIMITER;
    goto cleanup;
  }

  do {
    // 인자 문자열 시작지점
    int start = i;
    while(!isspace(ch=rawCmd[i]) && ch != ',' && ch != '\0') ++i;
    // 인자 문자열 경계 찾음!
    int argLen = i - start;

    // 다음 인자 시작으로 i를 업데이트 하면서 delimiter 검사를 함    
    ch = jumpBlank(rawCmd, &i);
    // if(ch == '\0') -> 문자열 끝 검사는 do-while condition 에서.
    if(ch != '\0' && ch != ',') {
      // 인자 문자열 시작점. 하지만 공백은 인자 delimiter 로 허용하지 않음 
      errorCode = INVALID_DELIMITER;
      goto cleanup;
    }
    if(ch == ',') {
      ++i;
      // delimiter 이후로 반드시 non-null, non-delimiter 문자가 나와야 함
      ch = jumpBlank(rawCmd, &i);
      if(ch == '\0' || ch == ',') {
        errorCode = INVALID_DELIMITER;
        goto cleanup;
      }
    }

    int nextPos = saveToBuffer(buffer, bufferPos, rawCmd + start, argLen);
    
    if(argc+1 > argcReserve) {
      argv = realloc(argv, argcReserve += 6);
    }
    argv[argc++] = buffer + bufferPos;
    bufferPos = nextPos;

  } while(ch != '\0');

  /* 리턴 전 Command 구조체에 포인터 저장 */
parse_end:
  pArgs->argc = argc;
  pArgs->argv = argv;
  return 0;

  /* 에러발생 시 리턴 전 수행목적 */ 
cleanup:
  free(argv);
  free(buffer);
  pArgs->argc = 0;
  pArgs->argv = NULL;
  return errorCode;
}

void deallocArguments(Arguments args) {
  free(args.argv[0]);
  free(args.argv);
  args.argc = 0;
  args.argv = NULL;
}

int saveToBuffer(char *buffer, int pos, char *src, int len) {
  buffer += pos;
  memcpy(buffer, src, len);
  buffer += len + 1;
  *buffer = '\0';
  return pos + len + 1;
}

char jumpBlank(char *str, int *pPos) {
  char lastChar;
  int i = *pPos;
  while(isspace(lastChar=str[i])) i++;
  *pPos = i;
  return lastChar;
}
