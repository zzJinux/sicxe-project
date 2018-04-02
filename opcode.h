/** opcode.h
 * opcodelist 관리 모듈 헤더
 */
#ifndef _OPCODE_H_
#define _OPCODE_H_

#include <stdio.h>
#include "./util.h"
#include "./shell-context.h"

#define MNEMONIC_MAXLEN 6

typedef enum _OPCODE_FORMAT {
  FORMAT1 = 1,
  FORMAT2 = 2,
  FORMAT34 = 3
} OPCODE_FORMAT;

typedef struct _OpcodeDef {
  char mn[MNEMONIC_MAXLEN+1];
  BYTE hex;
  OPCODE_FORMAT fm;
} OpcodeDef;

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
HashTable *initOpcodeList(FILE *stream);

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
OpcodeDef *findOpcode(HashTable *hashTable, char const *pat);

/** printOpcodeList
 *  opcode 해시테이블을 visual하게 출력
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void printOpcodeList(HashTable *hashTable);

/** cleanupOpcodeList
 *  opcode 해시테이블이 점유한 자원 해제
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void cleanupOpcodeList(HashTable *hashTable);

#endif
