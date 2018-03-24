#include "opcode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int hashing(char const *text) {
  unsigned a=1, b=0;
  char c;
  while((c=*text++) != '\0') {
    a = (a+c)%65521;
    b = (b+a)%65521;
  }
  return (b<<16|a)%HASH_TABLE_SIZE;
}

OpcodeHashNodePtr *initOpcodeList(ShellContextPtr pContext, FILE *stream) {
  int i;
  OpcodeHashNodePtr *ht = pContext->opcodeHashTable;
  for(i=0; i<HASH_TABLE_SIZE; ++i) {
    ht[i] = NULL;
  }

  int opcode;
  char mnemonic[MNEMONIC_MAXLEN+1];
  int h;
  while(fscanf(stream, " %x %s %*[^\n] ", &opcode, mnemonic) == 2) {
    h = hashing(mnemonic);
    OpcodeHashNodePtr *dpNode = ht + h;

    OpcodeHashNodePtr newNode, pNode;
    newNode = (OpcodeHashNodePtr)malloc(sizeof(OpcodeHashNode));
    if(newNode == NULL) {
      return NULL;
    }

    strcpy(newNode->mnemonic, mnemonic);
    newNode->opcode = opcode;
    newNode->next = NULL;

    pNode = *dpNode;
    if(pNode == NULL) {
      *dpNode = newNode;
    }
    else {
      while(pNode->next != NULL) pNode = pNode->next;
      pNode->next = newNode;
    }
  }
  
  return ht;
}

int findOpcode(ShellContextPtr pContext, char const *pat) {
  int h = hashing(pat);
  OpcodeHashNodePtr pNode = pContext->opcodeHashTable[h];
  while(pNode != NULL) {
    if(strcmp(pNode->mnemonic, pat) == 0) {
      return pNode->opcode;
    }
    pNode = pNode->next;
  }
  return -1;
}

void printOpcodeList(ShellContextPtr pContext) {
  int i;
  OpcodeHashNodePtr pNode;
  for(i=0; i<HASH_TABLE_SIZE; ++i) {
    printf("%2d : ", i);
    pNode = pContext->opcodeHashTable[i];
    if(pNode == NULL) {
      printf("empty\n");
      continue;
    }
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
    pNode = pContext->opcodeHashTable[i];
    while(pNode != NULL) {
      next = pNode->next;
      free(pNode);
      pNode = next;
    }
    pContext->opcodeHashTable[i] = NULL;
  }
}