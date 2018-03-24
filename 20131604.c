#include <stdio.h>
#include <string.h>
#include "shell-context.h"
#include "execute-cmd.h"

/* 입력 최대길이 */
#define MAX_LEN 256

/** flushRestInput
 *  입력이 길이 256(개행문자 제외)를 넘어설 경우 다음 입력에 영향을 끼치지 않도록
 *  남은 입력에 대해 개행문자가 나올 때까지 읽고 버린다.
 *  fgets 함수가 개행문자까지만 읽는 특징에 착안
 * 
 *  @인수
 *    last - 마지막으로 읽은 입력 문자열됨. 또한 임시버퍼로 사용됨.
 */
void flushRestInput(char *last);

int main() {
  // "opcode.txt" 으로부터 파일스트림을 가져옴. 실패 시 메시지 출력 후 프로그램 종료
  FILE *opcodeRaw;
  opcodeRaw = fopen("opcode.txt", "r");
  if(opcodeRaw == NULL) {
    printf("- error: file open failed\n");
    return -1;
  }

  // 프로그램의 상태를 저장/변경하는 ShellContext 구조체를 초기화 후 저장
  ShellContextPtr pContext = initShellContext(opcodeRaw);
  fclose(opcodeRaw);
  if(pContext == NULL) {
    printf("- error: memory allocation failed\n");
    return -1;
  }

  // executeCommand가 QUIT_SHELL을 반환할 때까지 명령입력을 계속 받는다.
  while(1) {
    printf("sicsim> ");
    // stdin에서 최대 MAX_LEN 길이의 커맨드 입력받음 (+1는 null 문자를 위함)
    char input[MAX_LEN+1];
    // 문자열 input 는 반드시 \0, \n\0 으로 끝남
    fgets(input, MAX_LEN+1, stdin);
    int len = strlen(input);

    Arguments args;
    // input을 executeCommand의 인수인 Arguments 구조체 타입으로 파싱
    PARSE_RESULT errCode = parseInput(&args, input, len);
    // 공백문자만 있거나 파싱에 실패하면 다음 입력을 받음
    if(errCode & PARSE_ERROR || errCode & COMMAND_EMPTY) {
      continue;
    }

    // history 커맨드는 입력받는 그대로의 문자열이 필요하므로 따로 기록한다
    args.RAW = input;
    args.RAW_LEN = len;

    // 실제 명령 수행의 진입점
    EXIT_FLAG exitFlag = executeCommand(pContext, args);
    // parseInput 함수가 args 변수의 내부 멤버에 수행한 동적할당을 해제
    deallocArguments(args);

    // QUIT_SHELL 플래그가 켜져 있으면 무한루프 탈출
    if(exitFlag & QUIT_SHELL) {
      puts("Exit sicsim...");
      break;
    }

    // 나머지 입력 제거
    flushRestInput(input);
  }

  // ShellContext 구조체 정리
  cleanupShellContext(pContext);
  return 0;
}

void flushRestInput(char *last) {
  int len = strlen(last);
  // 입력의 끝부분은 항상 개행문자
  while(last[len-1] != '\n') {
    fgets(last, len+1, stdin);
    len = strlen(last);
  }
}
