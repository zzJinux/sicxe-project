#ifndef _SHELL_CONTEXT_H_
#define _SHELL_CONTEXT_H_

#include "typedefs.h"

typedef struct _ShellContext {
  HistNodePtr histRootNode;
  HistNodePtr histCurrentNode;
  unsigned memSize;
  BYTE *memAddr;
  OFFSET memIdx;
} ShellContext, *ShellContextPtr;

ShellContextPtr initShellContext();

void cleanupShellContext(ShellContextPtr pContext);

#endif