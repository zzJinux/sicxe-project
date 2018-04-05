#ifndef _ASSEMBLE_TYPES_H_
#define _ASSEMBLE_TYPES_H_

#include "./assemble_consts.h"

typedef struct _Token {
  char const *tokenText;
  int colNo;
} Token;

typedef struct _Statement {
  char const *line;
  int lineNo;
  int loc;
  Token *label, *mnemonic, *operand;
} Statement;

typedef struct _MRec {
  OFFSET start;
  int halfBytes;
} MRec;

#define OBJCODE_BUF_LEN(baseT) ((TRECORD_CODE_LIMIT+sizeof(baseT)-1)/sizeof(baseT))

typedef struct _AssembleState {
  char programName[PROGRAM_NAME_LIMIT+1];
  int programLength;
  int startAddr;
  int baseAddr;
  int pcAddr; // different meaning along the contexts
  unsigned objcodeBuf[OBJCODE_BUF_LEN(unsigned)];
  int objcodeLen;
  enum _AssembleStateFlag flag;
  HashTable *optab, *symtab;

  char textRecBuf[TRECORD_CODE_LIMIT*2+1];
  int curTextRecordLen;
  Vec *mRecVec;
} AssembleState;

#endif