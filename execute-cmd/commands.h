#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "typedefs.h"
#include "shell-context.h"

/* Arguments, 입력 명령의 명령, 인자를 저장한 구초제 */
typedef struct _Arguments {
  char const *RAW; int RAW_LEN; // 입력 당시의 문자열을 기록해둠
 // argc: 인자의 총 개수(명령 포함)
 // argv: 인자들의 배열
  int argc; char **argv;
} Arguments;

/* EXIT_FLAG, 명령 실행 중/후 발생한 예외에 대응하는 상수를 지정한 열거형  */
typedef enum _EXIT_FLAG {
  QUIT_SHELL = 1<<0,
  UNKNOWN_COMMAND = 1<<1,
  UNKNOWN_ARGUMENT = 1<<2,
  OUT_OF_RANGE = 1<<3,
  NOT_HEX = 1<<4,
  ARGUMENT_TOO_LONG = 1<<5,
  INVALID_ARGUMENT = OUT_OF_RANGE | NOT_HEX | ARGUMENT_TOO_LONG,
  EXECUTE_ERROR =  UNKNOWN_COMMAND | UNKNOWN_ARGUMENT | INVALID_ARGUMENT
} EXIT_FLAG;

/* COMMAND_FUNC, 명령의 함수 타입 */
typedef EXIT_FLAG (*COMMAND_FUNC)(ShellContextPtr, const Arguments);

/* ShellCmd, 명령의 구성요소를 저장한 구조체 */
typedef struct _ShellCmd {
  // fullname: 명령의 이름
  // shorthand: 명령의 이름 단축형
  // desc: 명령에 대한 설명(사용법 등등)
  char const *fullname, *shorthand, *desc;
  COMMAND_FUNC commandFunc; // 명령과 대응되는 함수
} ShellCmd;

/** getShellCmdCount
 *  쉘에 미리 정의된 명령들의 개수를 반환
 * 
 *  @반환
 *    생략
 */
int getShellCmdCount(void);

/** getShellCmdList
 *  쉘에 미리 정의된 명령들의 리스트(배열)를 반환
 * 
 *  @반환
 *    생략
 */
ShellCmd const *getShellCmdList(void);

/** COMMAND_HELP, COMMAND_DIR, ... , COMMAND_OPCODELIST
 *  각각 help, dir, ... , opcodelist 명령들에 각각 대응되는 함수
 *  정의된 동작을 수행한다
 *  
 *  첫번째 인수는 프로그램 상태, 두번째 인수는 명령 인자 구조체
 *  모두 실행 중/후 발생한 예외를 반환한다.
 *    
 */
EXIT_FLAG COMMAND_HELP(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_DIR(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_QUIT(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_HISTORY(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_DUMP(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_EDIT(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_FILL(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_RESET(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_OPCODE(ShellContextPtr, const Arguments);
EXIT_FLAG COMMAND_OPCODELIST(ShellContextPtr, const Arguments);

#endif