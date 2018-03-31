#include "./util.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

LLNodePtr *initHashTable(int size) {
  LLNodePtr *table = malloc(size * sizeof(LLNodePtr));
  if(table == NULL) return NULL;

  int i;
  for(i=0; i<size; ++i) {
    table[i] = NULL;
  }

  return table;
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
int hash_adler32(char const *text, int mod) {
  unsigned a=1, b=0;
  char c;
  while((c=*text++) != '\0') {
    a = (a+c)%65521;
    b = (b+a)%65521;
  }
  return (b<<16|a) % mod;
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
  char const *begin = src;
  while(!isspace(*src) && !!*src) ++src;
  return src - begin;
}

char const *copyToken(char *dest, char const *src) {
  while(!isspace(*src) && !!*src) *dest++ = *src++;
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