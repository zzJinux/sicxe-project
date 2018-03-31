/** parse.c
 *  execute-cmd.h 의 parseInput, deallocArguments 함수 구현부
 */
#include "./execute-cmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./util.h"

/** saveToBuffer(내부함수)
 *  buffer + pos 위치부터 len 길이의 문자열을 src 로부터 복사 후
 *  buffer에 나중에 복사 시 복사 시작점 위치 반환
 * 
 *  @인수
 *    buffer - 저장할 버퍼 주소
 *    pos - buffer에 복사할 시작점
 *    src - 복사 원본 문자열
 *    len - 복사할 문자열의 길이
 * 
 *  @반환
 *    buffer에 복사할 다음 위치
 */
static int saveToBuffer(char *buffer, int pos, char *src, int len);

/** printErrMsg
 *  result 값에 따라 에러메시지를 출력함
 * 
 *  @인수
 *    result - 파싱 중/후 상태를 저장한 값
 */
static void printErrMsg(PARSE_RESULT result);

PARSE_RESULT parseInput(Arguments *pArgs, char *rawCmd, int cmdLength) {
  PARSE_RESULT errCode = 0;

  // 문자열 접근 시작점
  int i=0;
  // 마지막으로 접근한 위치의 문자를 저장함
  char ch;
  ch = jumpBlank(rawCmd, &i);

  // 공백 문자들을 건너뛴 후 문자열의 끝에 도달했으므로 빈 입력임
  if(ch == '\0') {
    return COMMAND_EMPTY;
  }

  /* 현재 상태: 공백 문자가 아닌 첫번째 문자, 명령이름의 시작점 */

  // 인자들의 배열을 저장할 변수 초기화
  char **argv = NULL;
  char *buffer = NULL;

  // 파싱된 인자들의 개수
  int argc = 0;
  // 인자가 얼마나 있을지 알 수 없으므로 6개까지 버퍼링하기 위한 면수
  int argcReserve = 6;
  // 문자열 배열 동적할당
  argv = (char **)malloc(argcReserve * sizeof(char *));
  // 동적할당 중에 문제 발생 시 NULL을 반환함 (이하 동일)
  if(argv == NULL) {
    // 에러 상수를 기록 후 바로 return 하지 않고 점유한 자원들에 대한 처리를 해야함
    errCode = ALLOCATION_ERROR;
    // return문과 비슷한 역할을 함
    goto cleanup;
  }


  /*
    * buffer 변수에 대한 설명 *

    문자열 배열, argv, 의 각 문자열은 따로따로 동적할당 되지 않는다.
    위에서 기술했듯 인자 문자열 길이의 총합을 알 수 있기 때문에
    모든 인자 문자열을 담을 수 있는 배열을 딱 한 번 할당하고
    각 인자 문자열들의 위치를 배열의 특정 위치로 지정한다.
    Null Terminator도 반드시 기록하므로 오버플로우날 일이 없고
    
    동적할당과 해제를 한 줄로 할 수 있다는 장점이 있다.

    예시: (@는 null terminator)
      입력된 문자열 : command arg1,lorem,ipsum,arg2@
      buffer(memory): command@arg1@lorem@ipsum@arg2@
    
    입력된 문자열 배열을 그대로 쓰지 않는 이유는
    나중에 최초 입력된 그대로의 문자열을 필요로 할 수 있기 때문.
   */

  // buffer에 문자열 저장할 현재 위치를 기록한 변수
  int bufferPos = 0;
  // 파싱 후 산출된 인자 문자열들의 길이(null문자 포함)의 총합은
  // 쉘에 입력된 문자열의 길이(null문자 포함)보다 반드시 크지 못함
  // delimiter + null문자 의 개수가 인자 문자열의 개수가 같기 때문
  buffer = (char *)malloc((cmdLength+1) * sizeof(char));
  // (이전 설명과 동일)
  if(buffer == NULL) {
    errCode = ALLOCATION_ERROR;
    goto cleanup;
  }

  // 명령의 이름, 즉, 0번째
  // 인자의 경계(소문자 알파벳의 연속 후 첫 공백 혹은 null문자)를 찾는다
  int start = i; // 0번째 인자의 시작점
  for(; !isspace(ch=rawCmd[i]) && ch != '\0'; ++i) {
    // 소문자가 아닌 문자가 중간에 있으면 에러로 간주
    if(ch < 'a' || ch > 'z') {
      errCode = NOT_LOWERCASE;
      goto cleanup;
    }
  }
  // 인자 문자열 경계 찾음
  // i == 명령 이름의 경계 (open boundary)

  int argLen = i - start; // 명령 이름의 길이
  // saveToBuffer 함수로 buffer에 저장하고 다음 저장위치를 기록
  int nextPos = saveToBuffer(buffer, bufferPos, rawCmd + start, argLen);

  // 예약된 인자 개수를 넘어설 경우 재할당
  if(argc+1 > argcReserve) {
    argv = realloc(argv, argcReserve += 6);
  }
  // 문자열 배열에 0번째 인자의 시작점을 기록하고 다음 인자 파싱으로 진행함
  argv[argc++] = buffer + bufferPos;
  bufferPos = nextPos;
  
  // 문자열의 끝에 도달했을 경우 파싱 정상종료
  if(ch == '\0') {
    goto parse_end;
  }

  // 0번째 인자 뒤에 있을 수 있는 공백들을 건너뜀
  ch = jumpBlank(rawCmd, &i);
  // 문자열의 끝에 도달했을 경우 파싱 정상종료
  if(ch == '\0') {
    goto parse_end;
  }
  // 0번째 인자와 1번째 인자 사이에는 delimiter가 올 수 없음, 에러처리
  if(ch == ',') {
    errCode = INVALID_DELIMITER;
    goto cleanup;
  }

  // 아래의 반복문은 위에서 한 파싱과 비슷하지만 조금씩 다름
  do {
    int start = i; // argc번째 인자의 시작점
    while(!isspace(ch=rawCmd[i]) && ch != ',' && ch != '\0') ++i;
    // 인자 문자열 경계 찾음, i는 인자의 경계
    int argLen = i - start; // 인자의 길이

    /* 바로 saveToBuffer를 수행하지 않고 뒤에 있는 delimiter 유효성 검사를 한다 */

    ch = jumpBlank(rawCmd, &i); // 공백 무시

    // 문자열의 끝이 아닌데 공백 뒤에 non-delimiter가 오면
    // delimiter 누락이므로 에러처리
    if(ch != '\0' && ch != ',') {
      errCode = INVALID_DELIMITER;
      goto cleanup;
    }

    // 연속된 delimiter 검사
    if(ch == ',') {
      ++i;
      // delimiter의 존재는 그 다음 인자의 존재의 충분조건
      ch = jumpBlank(rawCmd, &i);
      // 그 다음 인자의 시작점이 아닌 경우 잘못된 delimiter임. 에러처리
      if(ch == '\0' || ch == ',') {
        errCode = INVALID_DELIMITER;
        goto cleanup;
      }
    }

    // delimiter 유효성 검사가 끝난 후 saveToBuffer 수행
    // 다음에 나올 statement 들도 이미 등장한 패턴임
    int nextPos = saveToBuffer(buffer, bufferPos, rawCmd + start, argLen);
    
    if(argc+1 > argcReserve) {
      argv = realloc(argv, argcReserve += 6);
    }
    argv[argc++] = buffer + bufferPos;
    bufferPos = nextPos;

  } while(ch != '\0');
  // 에러없이 문자열 끝에 도달함
  // goto parse_end 와 같은 효과임

  // 파싱 성공 후 argc, argv를 Arguments 구조체에 저장
parse_end:
  pArgs->argc = argc;
  pArgs->argv = argv;
  return 0;

  // 에러 발생 시 지금까지 수행한 동적할당을 해제하고
  // Arguments 구조체에 파싱 실패(인자가 0개)를 기록함
  // 그 후 에러메시지 출력 후 리턴
cleanup:
  free(argv);
  free(buffer);
  pArgs->argc = 0;
  pArgs->argv = NULL;
  printErrMsg(errCode);
  return errCode;
}

void deallocArguments(Arguments args) {
  // parseInput에서 수행한 동적할당 방식에 맞추어 free 2개로 해제를 함
  free(args.argv[0]);
  free(args.argv);
  args.argc = 0;
  args.argv = NULL;
}

int saveToBuffer(char *buffer, int pos, char *src, int len) {
  buffer += pos; // buffer(로컬)를 복사받을 시작점으로 계산
  memcpy(buffer, src, len); // 메모리 복사
  buffer += len; // null 문자를 저장할 위치로 계산
  *buffer = '\0'; // null-terminator 기록
  return pos + len + 1; // 다음 복사 위치, null-terminator 다음 위치를 반환
}

static void printErrMsg(PARSE_RESULT result) {
  // 에러플래그가 없으므로 그대로 종료
  if(result == 0) return;

  // AND 연산자로 플래그를 검사한 후 에러메시지 출력
  printf("-shell: ");
  if(result & INVALID_FORMAT) {
    printf("syntax error, ");
    if(result & NOT_LOWERCASE) {
      printf("command name is lowercase only");
    }
    else if(result & INVALID_DELIMITER) {
      printf("only ',' is allowed for the delimiter");
    }
    else {
      printf("<unknown>");
    }
    putchar('\n');
  }
  else if(result & ALLOCATION_ERROR) {
    printf("allocation error\n");
  }
  else {
    printf("<unknown error>\n");
  }
}