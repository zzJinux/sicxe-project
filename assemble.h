#ifndef _ASSEMBLE_H_
#define _ASSEMBLE_H_
#include <stdio.h>
#include <stdbool.h>
#include "./shell-context.h"
#include "./typedefs.h"
#include "./util.h"

#include "./assemble_types.h"
#include "./assemble_errs.h"

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

ASSEMBLE_ERROR assemble_pass1(FILE *asmIn, HashTable *optab, HashTable *symtab, Vec *stVec);

ASSEMBLE_ERROR assemble_pass2(FILE *lstOut, FILE *objOut, HashTable *optab, HashTable *symtab, Vec *stVec);

/** _assemble_printErrMsg
 *  assemble 중 발생한 오류의 메시지 출력
 *
 *  @인자
 *    code - assemble 에러코드
 *    detail - 에러 세부내용
 *
 */
void _assemble_printErrMsg(ASSEMBLE_ERROR code, char const* detail);

/** _assemble_printSynaxErrMsg (static)
 *  assemble 중 발생한 문법 오류의 메시지 출력
 *
 *  @인자
 *    code - assemble 에러코드
 *    st - 오류가 발생한 Statement
 *    colNo - 오류가 발생한 열번호
 *
 */
void _assemble_printSyntaxErrMsg(SYNTAX_ERROR code, Statement *st, int colNo);

#endif