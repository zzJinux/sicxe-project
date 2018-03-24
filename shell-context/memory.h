#ifndef _SIM_MEMORY_H_
#define _SIM_MEMORY_H_

#include "shell-context.h"

void *initMemory(ShellContextPtr pContext, int memSize);

void dumpMemory(ShellContextPtr pContext, int argc, ...);

void editMemory(ShellContextPtr pContext, OFFSET pos, BYTE value);
void fillMemory(ShellContextPtr pContext, OFFSET start, OFFSET end, BYTE value);
void resetMemory(ShellContextPtr pContext);

void cleanupMemory(ShellContextPtr pContext);

#endif