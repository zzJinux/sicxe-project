/** execute.c
 *  execute-cmd.h 의 executeCommand 함수 구현부
 */
#include "./execute-cmd.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "./shell-context.h"
#include "./history.h"
#include "./commands.h"

/** printErrMsg(내부함수)
 *  exitFlag 값에 따라 에러메시지를 출력함
 * 
 *  @인수
 *    exitFlag - COMMAND_FUNC 타입 함수가 반환한 값
 *    pCmd - exitFlag를 반환한 명령
 */
static void printErrMsg(EXIT_FLAG exitFlag, ShellCmd const *pCmd);

/** matchCommand(내부함수)
 *  문자열 str과 cmd와 대응되는 명령 문자열이 일치하는지 검사
 * 
 *  @인수
 *    str - 테스트할 문자열
 *    cmd - 비교 대상 명령
 * 
 *  @반환
 *    일치하면 1, 그 외에는 0
 */
static int matchCommand(char const *str, ShellCmd cmd) {
  // 명령의 단축형태와도 비교한다
  return strcmp(str, cmd.fullname) == 0
    || (cmd.shorthand != NULL && strcmp(str, cmd.shorthand) == 0);
}

EXIT_FLAG executeCommand(ShellContextPtr pContext, Arguments args) {
  // commands.c에 정의된 전체 명령리스트에 관한 정보를 저장
  // cmdFound는 매칭된 명령을 기록하기 위한 변수
  int cmdCnt = getShellCmdCount();
  ShellCmd const *cmdList = getShellCmdList(), *cmdFound = NULL;

  // 아래의 루프에서 매칭되는 명령어가 없을 경우, 이 초기값이 유지된다
  EXIT_FLAG exitFlag = UNKNOWN_COMMAND;
  // 명령의 이름
  char const *cmdText = args.argv[0];

  int i;
  for(i=0; i<cmdCnt; ++i) {
    if(matchCommand(cmdText, cmdList[i])) {
      // 일치하면 일치된 명령 구조체와 exitFlag를 기록한다
      cmdFound = cmdList + i;
      exitFlag = cmdFound->commandFunc(pContext, args);
      break;
    }
  }

  // exitFlag & EXECUTE_ERROR는 명령 수행 후 에러를 검사하는 식
  // history 명령은 자체적으로 history 리스트에 기록하므로 넘긴다
  if(!(exitFlag & EXECUTE_ERROR) && cmdFound->commandFunc != COMMAND_HISTORY) {
    updateHistory(pContext, args.RAW, args.RAW_LEN);
  }

  // exitFlag와 대응되는 에러메시지 출력
  printErrMsg(exitFlag, cmdFound);
  return exitFlag;
}

static void printErrMsg(EXIT_FLAG exitFlag, ShellCmd const *pCmd) {
  // 에러플래그가 없거나(0), 쉘 종료 메시지의 경우 에러가 아니므로 종료
  if(exitFlag == 0 || exitFlag & QUIT_SHELL) return;

  // AND 연산자로 플래그를 검사한 후 에러메시지 출력
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
  else if(exitFlag & INTERNAL_COMMAND_ERROR) {
    // empty
  }
  else {
    printf("<unknown error>");
  }
  putchar('\n');
}