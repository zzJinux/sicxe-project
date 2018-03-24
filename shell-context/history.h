#ifndef _HISTORY_H_
#define _HISTORY_H_

#include "shell-context.h"

void initHistory(ShellContextPtr pContext);

HistNodePtr updateHistory(ShellContextPtr pContext, char const* text, int leng);

void cleanupHistory(ShellContextPtr pContext);

void printHistory(ShellContextPtr pContext);

#endif