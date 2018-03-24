/** opcode.c
 *  opcode.h 구현
 */
#include "opcode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** hashing (내부 함수)
 *  문자열 해싱 함수
 * 
 *  @인수
 *    text - 해싱할 문자열
 * 
 *  @반환
 *    text 문자열의 해시값
 */
static int hashing(char const *text) {
  unsigned a=1, b=0;
  char c;
  while((c=*text++) != '\0') {
    a = (a+c)%65521;
    b = (b+a)%65521;
  }
  return (b<<16|a)%HASH_TABLE_SIZE;
}

// 동적할당 중에 문제 발생 시 NULL을 반환함
OpcodeHashNodePtr *initOpcodeList(ShellContextPtr pContext, FILE *stream) {
  int i;
  // 해시테이블 bucket 초기화
  OpcodeHashNodePtr *ht = pContext->opcodeHashTable;
  for(i=0; i<HASH_TABLE_SIZE; ++i) {
    ht[i] = NULL;
  }

  int opcode;
  char mnemonic[MNEMONIC_MAXLEN+1];
  int h;
  // stream 으로부터 opcode, mnemonic 을 입력받음 format은 무시(추후 구현)
  // fscanf 의 반환값은 성공적으로 입력받은 포맷입력의 개수, 2개를 입력받지 못하면
  // 입력의 끝으로 간주하고 종료한다
  while(fscanf(stream, " %x %s %*[^\n] ", &opcode, mnemonic) == 2) {
    h = hashing(mnemonic);
    // dpNode에 노드가 들어갈 해시테이블의 bucket의 포인터를 저장
    OpcodeHashNodePtr *dpNode = ht + h;

    OpcodeHashNodePtr newNode, pNode;
    // 새 노드 동적할당
    newNode = (OpcodeHashNodePtr)malloc(sizeof(OpcodeHashNode));
    if(newNode == NULL) {
      return NULL;
    }

    // 새 노드 멤버변수 초기화
    strcpy(newNode->mnemonic, mnemonic);
    newNode->opcode = opcode;
    newNode->next = NULL;

    // 만약 리스트의 루트가 NULL 이면 새 노드를 루트로 삼는다
    pNode = *dpNode;
    if(pNode == NULL) {
      *dpNode = newNode;
    }
    // 만약 루트가 있다면 리스트의 끝에 새 노드를 달아놓음
    else {
      while(pNode->next != NULL) pNode = pNode->next;
      pNode->next = newNode;
    }
  }
  
  return ht;
}

int findOpcode(ShellContextPtr pContext, char const *pat) {
  int h = hashing(pat);
  // 해시값으로부터 bucket 을 받아옴
  OpcodeHashNodePtr pNode = pContext->opcodeHashTable[h];
  // 일치하는 mnemonic이 나올 때까지 bucket의 리스트를 순회
  while(pNode != NULL) {
    if(strcmp(pNode->mnemonic, pat) == 0) {
      // 일치하면 해당하는 opcode 반환
      return pNode->opcode;
    }
    pNode = pNode->next;
  }

  // 찾지 못한 경우 -1 반환
  return -1;
}

void printOpcodeList(ShellContextPtr pContext) {
  int i;
  OpcodeHashNodePtr pNode;
  for(i=0; i<HASH_TABLE_SIZE; ++i) {
    // 해시테이블의 각 bucket 출력
    printf("%2d : ", i);

    // pNode 에 bucket의 리스트의 루트를 받아옴
    pNode = pContext->opcodeHashTable[i];
    if(pNode == NULL) {
      printf("empty\n");
      continue;
    }

    // 리스트의 끝까지 출력
    while(1) {
      printf("[%s,%02X]", pNode->mnemonic, pNode->opcode);
      pNode = pNode->next;
      if(pNode == NULL) break;
      printf(" -> ");
    }
    putchar('\n');
  }
}

void cleanupOpcodeList(ShellContextPtr pContext) {
  int i;
  OpcodeHashNodePtr pNode, next;
  for(i=0; i<HASH_TABLE_SIZE; ++i) {
    // pNode 에 bucket의 리스트의 루트를 받아옴
    pNode = pContext->opcodeHashTable[i];

    // 모든 리스트의 노드에 접근 후 동적할당 해제
    while(pNode != NULL) {
      next = pNode->next;
      free(pNode);
      pNode = next;
    }
    // 할당해제 후 무효화된 값을 덮어씌움
    pContext->opcodeHashTable[i] = NULL;
  }
}