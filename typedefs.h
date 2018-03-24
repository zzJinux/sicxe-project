#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#define MNEMONIC_MAXLEN (6) 
#define HASH_TABLE_SIZE (20)

typedef unsigned OFFSET;
typedef unsigned char BYTE;

typedef struct _HistNode {
  char *text;
  struct _HistNode *next;
} HistNode, *HistNodePtr;

typedef struct _OpcodeHashNode {
  char mnemonic[MNEMONIC_MAXLEN+1];
  BYTE opcode;
  struct _OpcodeHashNode *next;
} OpcodeHashNode, *OpcodeHashNodePtr;

#endif