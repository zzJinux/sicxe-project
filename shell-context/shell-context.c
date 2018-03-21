#include "shell-context.h"

#include "stdlib.h"
#include "history.h"

ShellContextPtr initShellContext() {
  ShellContextPtr pContext = (ShellContextPtr)malloc(sizeof(ShellContext));
  if(pContext == NULL) {
    return NULL;
  }
  initHistory(pContext);

  return pContext;
}

void cleanupShellContext(ShellContextPtr pContext) {
  cleanupHistory(pContext);
  free(pContext);
}
