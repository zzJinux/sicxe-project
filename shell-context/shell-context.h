#ifndef _SHELL_CONTEXT_H_
#define _SHELL_CONTEXT_H_

#include <stdio.h>
#include "typedefs.h"

typedef struct _ShellContext {
  HistNodePtr histRootNode;
  HistNodePtr histCurrentNode;

  unsigned memSize;
  BYTE *memAddr;
  OFFSET memIdx;

  OpcodeHashNodePtr opcodeHashTable[HASH_TABLE_SIZE];
} ShellContext, *ShellContextPtr;

ShellContextPtr initShellContext(FILE *stream);

void cleanupShellContext(ShellContextPtr pContext);

#endif