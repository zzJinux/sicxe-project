#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include "./typedefs.h"

typedef void (*CLEANUP_FUNC)(void *);

typedef struct _LinkedListNode {
  void *key;
  struct _LinkedListNode *next;
} LLNode, *LLNodePtr;

typedef struct _HashTable {
  int size;
  LLNodePtr *buckets;
} HashTable;

HashTable *initHashTable(int size);

void cleanupHashTable(HashTable *table, void (*cleanupKey)(void *));

unsigned hash_adler32(char const *text, int mod);

/** parseHex_u20
 *  hex에 저장된 문자열을 hexadecimal로 간주하고 최대 20bit의 정수로 파싱
 * 
 *  @인수
 *    hex - 파싱할 hexadecimal 문자열
 *    p - 계산결과 반환 포인터
 * 
 *  @반환
 *    파싱 중 발생한 에러. 20bit를 넘는 정수거나 미리 정의된 포맷이 아닌 경우가 있음
 */
NUMBER_PARSE_ERROR parseHex_u20(char const *hex, unsigned *p);

/** jumpBlank
 *  연속된 공백문자들을 건너뛰는 함수
 * 
 *  @인수
 *    str - 대상 문자열
 *    pPos - 연속 공백 문자열의 시작 인덱스 (반환용 포인터)
 * 
 *  @반환
 *    마지막 공백문자 다음의 문자
 */
char jumpBlank(char *str, int *pPos);

char *freadLine(FILE *stream);

int getTokenSize(char const *src);
char const *copyToken(char *dest, char const *src);
char *readToken(char const *text, int *pLen);

#endif