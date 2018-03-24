/** execute-cmd.h
 * 입력 파싱 및 명령 실행 모듈 헤더
 * 구현부는 execute.c 와 parse.c 로 나뉨
 */
#ifndef _EXECUTE_CMD_H_
#define _EXECUTE_CMD_H_

#include "typedefs.h"
#include "commands.h"

// PARSE_RESULT, 파싱 중 발생한 예외에 대응하는 상수를 지정한 열거형
typedef enum _PARSE_RESULT {
  COMMAND_EMPTY = 1<<0,
  NOT_LOWERCASE = 1<<1,
  INVALID_DELIMITER = 1<<2,
  ALLOCATION_ERROR = 1<<3,
  INVALID_FORMAT = NOT_LOWERCASE | INVALID_DELIMITER,
  PARSE_ERROR = INVALID_FORMAT | ALLOCATION_ERROR
} PARSE_RESULT;

/** parseInput
 *  rawCmd 문자열을 파싱하여 pArgs가 가리키는 Arguments 구조체에 결과를 저장함
 * 
 *  @인수
 *    pArgs - 파싱 결과를 저장한 Arguments 구조체의 포인터
 *    rawCmd - 파싱할 입력 문자열
 *    cmdLength - rawCmd의 길이
 * 
 *  @반환
 *    파싱 중 발생한 예외
 */
PARSE_RESULT parseInput(Arguments *pArgs, char *rawCmd, int cmdLength);

/** deallocArguments
 *  parseInput에서 수행한 자원할당을 해제하는 함수
 * 
 *  @인수
 *    args - 해제할 자원이 저장된 Arguments 구조체
 */
void deallocArguments(Arguments args);

/** executeCommand
 *  args 구조체로부터 명령 정보를 읽어 실행하는 함수(진입점)
 * 
 *  @인수
 *    pContext - 프로그램 상태
 *    args - 명령인자 구조체
 * 
 *  @반환
 *    실행 중/후 발생한 예외, 자세한 상수는 commands.h 에 정의돼있음
 */
EXIT_FLAG executeCommand(ShellContextPtr pContext, Arguments args);

#endif
