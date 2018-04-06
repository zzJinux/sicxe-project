#ifndef _ASSEMBLE_H_
#define _ASSEMBLE_H_
#include <stdio.h>
#include "./shell-context.h"
#include "./typedefs.h"

/* assemble 에 관련된 파일의 확장자 */
#define DEFAULT_ASM_EXT "asm"
#define DEFAULT_LST_EXT "lst"
#define DEFAULT_OBJ_EXT "obj"

/** assemble
 *  입력 assembly 소스를 assemble 함
 *   asmIn 스트림은 입력 assembly 소스,
 *   lstOut 스트림은 출력 listing 파일,
 *   objOut 스트림은 출력 obj 파일에 대응됨
 *
 *  @인자
 *    pContext - 쉘의 context
 *    asmIn - ...
 *    lstOut - ...
 *    objOut - ...
 *
 *  @반환
 *    assemble 중 발생한 커맨드 레벨의 에러코드
 *
 * */
ERROR_CODE assemble(ShellContextPtr pContext, FILE *asmIn, FILE *lstOut, FILE *objOut);

#endif
