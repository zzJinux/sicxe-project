/** opcode.h
 * opcodelist 관리 모듈 헤더
 */
#ifndef _OPCODE_H_
#define _OPCODE_H_

#include <stdio.h>
#include "./shell-context.h"

/** initOpcodeList
 *  opcode 해시테이블 초기화 함수
 * 
 *  @인수
 *    pContext - 프로그램 상태
 *    stream - opcode 정보를 읽어올 스트림
 *
 *  @반환
 *    에러 발생 시 NULL, 성공 시 해시테이블 포인터
 */
OpcodeHashNodePtr *initOpcodeList(ShellContextPtr pContext, FILE *stream);

/** findOpcode
 *  mnemonic의 opcode를 찾는 함수
 * 
 *  @인수
 *    pContext - 프로그램 상태
 *    pat - 검색할 mnemonic
 * 
 *  @반환
 *    찾은 mnemonic의 opcode, 실패 시 -1
 */
int findOpcode(ShellContextPtr pContext, char const *pat);

/** printOpcodeList
 *  opcode 해시테이블을 visual하게 출력
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void printOpcodeList(ShellContextPtr pContext);

/** cleanupOpcodeList
 *  opcode 해시테이블이 점유한 자원 해제
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void cleanupOpcodeList(ShellContextPtr pContext);

#endif
