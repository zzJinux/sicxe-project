/**
 * 전역적으로 사용되는 typedef문을 모아놓은 헤더
 */
#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#define MNEMONIC_MAXLEN (6) 
#define HASH_TABLE_SIZE (20)

typedef unsigned OFFSET;
typedef unsigned char BYTE;

/* HistNode, history 의 LinkedList 노드 */
typedef struct _HistNode {
  char *text;
  struct _HistNode *next;
} HistNode, *HistNodePtr;

/* OpcodeHashNode, opcode 해시테이블의 bucket 구성 노드 */
typedef struct _OpcodeHashNode {
  char mnemonic[MNEMONIC_MAXLEN+1];
  BYTE opcode;
  struct _OpcodeHashNode *next;
} OpcodeHashNode, *OpcodeHashNodePtr;

#endif