#include "./util.h"

#include <stdio.h>
#include <string.h>
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

void emptyHashTable(HashTable *table, void (*cleanupKey)(void *)) {
  if(table == NULL) return;
  int i, size;
  for(i=0, size=table->size; i<size; ++i) {
    LLNodePtr p = table->buckets[i], n;
    while(p != NULL) {
      n = p->next;
      cleanupKey(p->key);
      free(p);
      p = n;
    }
    table->buckets[i] = NULL;
  }
}

void cleanupHashTable(HashTable *table, void (*cleanupKey)(void *)) {
  if(table == NULL) return;
  emptyHashTable(table, cleanupKey);
  free(table->buckets);
  free(table);
}

char jumpBlank(char const *str, int *pPos) {
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

#define VEC_INC_AMOUNT (32)

Vec *initVec(int initSize, CLEANUP_FUNC cb) {
  Vec *vec = malloc(sizeof(Vec));
  void **arr = malloc(initSize * sizeof(void *));
  if(vec == NULL || arr == NULL) {
    return NULL;
  }

  vec->size = 0;
  vec->_rsize = initSize;
  vec->arr = arr;
  vec->cleanupItem = cb;

  return vec;
}

Vec *vecPush(Vec *vec, void *item) {
  if(vec->size + 1 > vec->_rsize) {
    vec->_rsize += VEC_INC_AMOUNT;
    vec->arr = realloc(vec->arr, vec->_rsize * sizeof(void *));
    if(vec->arr == NULL) {
      return NULL;
    }
  }

  vec->arr[vec->size++] = item;
  return vec;
}

void cleanupVec(Vec *vec) {
  if(vec == NULL) return;
  int i, size;
  for(i=0, size=vec->size; i<size; ++i) {
    vec->cleanupItem(vec->arr[i]);
  }
  free(vec->arr);
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
    if(len+1 > reserved) {
      str = realloc(str, reserved+=incSize);
      if(str == NULL) return NULL;
    }
  }
  str = realloc(str, len+1);
  str[len] = 0;

  return str;
}

int getTokenSize(char const *src) {
  char ch;
  char const *begin = src;
  while((ch=*src) != '\n' && ch) {
    // string literal quote detected
    if(ch == '\'') {
      ++src;
      while((ch=*src) != '\'' && ch) ++src;
      if(!ch) {
        return -1;
      }
    }
    else if(isspace(ch) || ch == ',') {
      int u=1;
      if(isspace(ch)) {
        ch = jumpBlank(src, &u);
        if(ch != ',') { break; }
        ++u;
      }
      ch = jumpBlank(src, &u);
      if(ch == ',' || !ch) {
        return -1;
      }
      src += u;
      continue;
    }

    ++src;
  }

  return src - begin;
}

int findToken(char const *text, char delim, int *i_r, int *len_r) {
  char ch;
  int i = 0;

  ch = jumpBlank(text, &i);
  if(ch == '\0' || ch == delim) {
    if(i_r != NULL) *i_r = i;
    if(len_r != NULL) *len_r = 0;
    return -1;
  }

  int start = i;
  while(!isspace(ch=text[i]) && ch != delim && ch != '\0') ++i;
  // 인자 문자열 경계 찾음
  if(i_r != NULL) *i_r = start;
  if(len_r != NULL) *len_r = i - start;

  // delimiter 유효성 검사 시작
  ch = jumpBlank(text, &i);

  if(ch != '\0' && ch != delim) {
    // delimiter 누락
    return -1;
  }

  // 연속된 delimiter 검사
  if(ch == delim) {
    ++i;
    ch = jumpBlank(text, &i);
    if(ch == '\0' || ch == delim) {
      return -1;
    }

    // next token
    return i;
  }

  // null
  return 0;
}