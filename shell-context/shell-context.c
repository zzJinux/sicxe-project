#include "shell-context.h"

#include "stdlib.h"
#include "history.h"
#include "memory.h"

#define MEM_TOTAL (1u<<20)

ShellContextPtr initShellContext() {
  ShellContextPtr pContext = (ShellContextPtr)malloc(sizeof(ShellContext));
  if(pContext == NULL) {
    return NULL;
  }

  initHistory(pContext);
  void *memStart = initMemory(pContext, MEM_TOTAL);
  if(memStart == NULL) {
    return NULL;
  }

  return pContext;
}

void cleanupShellContext(ShellContextPtr pContext) {
  cleanupHistory(pContext);
  cleanupMemory(pContext);
  free(pContext);
}
