/** opcode.c
 *  opcode.h 구현
 */
#include "./opcode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./util.h"

#define OPCODE_TABLE_SIZE 20

static LLNodePtr createNode(BYTE opcode, char const mnemonic[], OPCODE_FORMAT fm) {
  LLNodePtr p = malloc(sizeof(LLNode));
  OpcodeDef *key = malloc(sizeof(OpcodeDef));
  if(p == NULL || key == NULL) return NULL;

  strcpy(key->mn, mnemonic);
  key->hex = opcode;
  key->fm = fm;

  p->key = key;
  p->next = NULL;
  return p;
}

// 동적할당 중에 문제 발생 시 NULL을 반환함
HashTable *initOpcodeList(FILE *stream) {

  // 해시테이블 bucket 초기화
  HashTable *ht = initHashTable(20);
  if(ht == NULL) {
    return NULL;
  }
  // OpcodeHashNodePtr *ht = pContext->opcodeHashTable;
  // for(i=0; i<HASH_TABLE_SIZE; ++i) {
    // ht[i] = NULL;
  // }

  int opcode;
  char mnemonic[MNEMONIC_MAXLEN+1];
  int format;
  // stream 으로부터 opcode, mnemonic 을 입력받음 format은 무시(추후 구현)
  // fscanf 의 반환값은 성공적으로 입력받은 포맷입력의 개수, 2개를 입력받지 못하면
  // 입력의 끝으로 간주하고 종료한다
  while(fscanf(stream, " %x %s %d%*[^\n] ", &opcode, mnemonic, &format) == 3) {
    int h = hash_adler32(mnemonic, OPCODE_TABLE_SIZE);
    // dpNode에 노드가 들어갈 해시테이블의 bucket의 포인터를 저장
    LLNodePtr *dpNode = ht->buckets + h;

    LLNodePtr newNode, pNode;
    // 새 노드 동적할당
    newNode = createNode((BYTE)opcode, mnemonic, format == 1 ? FORMAT1 : format == 2 ? FORMAT2 : FORMAT34);
    if(newNode == NULL) {
      return NULL;
    }

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

OpcodeDef *findOpcode(HashTable *hashTable, char const *pat) {
  int h = hash_adler32(pat, OPCODE_TABLE_SIZE);
  // 해시값으로부터 bucket 을 받아옴
  LLNodePtr pNode = hashTable->buckets[h];
  // 일치하는 mnemonic이 나올 때까지 bucket의 리스트를 순회
  while(pNode != NULL) {
    OpcodeDef *key = pNode->key;
    if(strcmp(key->mn, pat) == 0) {
      // 일치하면 해당하는 OpcodeDef *반환
      return key;
    }
    pNode = pNode->next;
  }

  // 찾지 못한 경우 -1 반환
  return NULL;
}

void printOpcodeList(HashTable *hashTable) {
  int i;
  LLNodePtr pNode;
  for(i=0; i<hashTable->size; ++i) {
    // 해시테이블의 각 bucket 출력
    printf("%2d : ", i);

    // pNode 에 bucket의 리스트의 루트를 받아옴
    pNode = hashTable->buckets[i];
    if(pNode == NULL) {
      printf("empty\n");
      continue;
    }

    // 리스트의 끝까지 출력
    while(1) {
      OpcodeDef *key = pNode->key;
      printf("[%s,%02X]", key->mn, key->hex);
      pNode = pNode->next;
      if(pNode == NULL) break;
      printf(" -> ");
    }
    putchar('\n');
  }
}

void cleanupOpcodeList(HashTable *hashTable) {
  cleanupHashTable(hashTable, free);
}