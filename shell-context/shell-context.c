#include "shell-context.h"

#include <stdlib.h>
#include "history.h"
#include "memory.h"
#include "opcode.h"

#define MEM_TOTAL (1u<<20)

ShellContextPtr initShellContext(FILE *stream) {
  ShellContextPtr pContext = (ShellContextPtr)malloc(sizeof(ShellContext));
  if(pContext == NULL) {
    return NULL;
  }

  initHistory(pContext);
  BYTE *memStart = initMemory(pContext, MEM_TOTAL);
  if(memStart == NULL) {
    return NULL;
  }

  OpcodeHashNodePtr *pNodeList = initOpcodeList(pContext, stream);
  if(pNodeList == NULL) {
    return NULL;
  }

  return pContext;
}

void cleanupShellContext(ShellContextPtr pContext) {
  cleanupHistory(pContext);
  cleanupMemory(pContext);
  cleanupOpcodeList(pContext);
  free(pContext);
}
