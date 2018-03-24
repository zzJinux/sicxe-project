#ifndef _OPCODE_H_
#define _OPCODE_H_

#include <stdio.h>
#include "shell-context.h"

OpcodeHashNodePtr *initOpcodeList(ShellContextPtr pContext, FILE *stream);

int findOpcode(ShellContextPtr pContext, char const *pat);

void printOpcodeList(ShellContextPtr pContext);

void cleanupOpcodeList(ShellContextPtr pContext);

#endif
