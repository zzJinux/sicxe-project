#ifndef _SHELL_CONTEXT_H_
#define _SHELL_CONTEXT_H_

#include "typedefs.h"

ShellContextPtr initShellContext();

void cleanupShellContext(ShellContextPtr pContext);

#endif