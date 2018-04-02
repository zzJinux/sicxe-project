#include "./util.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "typedefs.h"

HashTable *initHashTable(int size) {
  HashTable *ht = malloc(sizeof(HashTable));
  LLNodePtr *buckets = malloc(size * sizeof(LLNodePtr));
  if(ht == NULL || buckets == NULL) return NULL;

  int i;
  for(i=0; i<size; ++i) {
    buckets[i] = NULL;
  }
  ht->size = size;
  ht->buckets = buckets;

  return ht;
}

void cleanupHashTable(HashTable *table, void (*cleanupKey)(void *)) {
  int i, size;
  for(i=0, size=table->size; i<size; ++i) {
    LLNodePtr p = table->buckets[i], n;
    while(p != NULL) {
      n = p->next;
      cleanupKey(p->key);
      free(p);
      p = n;
    }
  }
  free(table);
}

char jumpBlank(char *str, int *pPos) {
  char lastChar;
  int i = *pPos;
  // 마지막 문자를 저장하면서 공백이 안나올 때까지 i 증가
  while(isspace(lastChar=str[i])) i++;
  *pPos = i; // i 반환
  return lastChar;
}

/** hash_adler32
 *  adler32 알고리즘을 사용하는 해시함수.
 * 
 *  @인수
 *    text - 해싱할 문자열
 *    mod - hash 범위 reduction 상수
 * 
 *  @반환
 *    [0, mod)으로 reduce된 해시값
 */
unsigned hash_adler32(char const *text, int mod) {
  unsigned a=1, b=0;
  char c;
  while((c=*text++) != '\0') {
    a = (a+c)%65521;
    b = (b+a)%65521;
  }
  return (b<<16|a) % mod;
}

NUMBER_PARSE_ERROR parseHex_u20(char const *hex, unsigned *p) {
  unsigned x = 0;
  char c;
  int i;
  // hexadecimal 값 계산
  for(i=0; isxdigit(c=hex[i]) && i<5; ++i) {
    x = x*16 + c - (c < 'A' ? '0' : (c < 'a' ? 'A' : 'a') - 10); 
  }
  if(c != '\0') {
    // 20bit를 파싱하고도 문자열이 안끝났으므로 너무 큰 값임
    if(i>=5) return NUMBER_TOO_LONG;
    // 중간에 [0-9a-zA-Z] 가 아닌 값이 있음
    else return INVALID_NUMBER_FORMAT;
  }
  *p = x;
  return 0;
}

char *freadLine(FILE* stream) {
  int incSize = 16, reserved = 64, len = 0;
  int ch;

  char *str = malloc(reserved);
  if(str == NULL) return NULL;
  while((ch=fgetc(stream)) != '\n' && ch != EOF) {
    str[len++] = ch;
    if(len+1 >= reserved) {
      str = realloc(str, reserved+=incSize);
      if(str == NULL) return NULL;
    }
  }
  if(ch == '\n') fgetc(stream);
  str = realloc(str, len+1);
  str[len] = 0;

  return str;
}

int getTokenSize(char const *src) {
  int inQ = 0;
  char ch;
  char const *begin = src;
  while((!isspace(ch=*src) || inQ) && !!ch) {
    // string literal quote detected
    if(ch == '\'') {
      inQ = !inQ;
    }
    ++src;
  }
  return src - begin;
}

char const *copyToken(char *dest, char const *src) {
  int inQ = 0;
  char ch;
  while((!isspace(ch=*src) || inQ) && !!*src) {
    if(ch == '\'') {
      inQ = !inQ;
    }
    *dest++ = *src++;
  }
  *dest = '\0';
  return src;
}

char *readToken(char const *text, int *pLen) {
  int len = getTokenSize(text);
  char *str = malloc(len+1);
  copyToken(str, text);
  *pLen = len;
  return str;
}
