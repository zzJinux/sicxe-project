#include "memory.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define DUMP_COL 16
#define DEFAULT_BYTES 160

void *initMemory(ShellContextPtr pContext, int memSize) {
  void *memStart = malloc(memSize);
  if(memStart == NULL) {
    return NULL;
  }

  pContext->memSize = memSize;
  pContext->memAddr = memStart;
  pContext->memIdx = 0;

  return memStart;
}

// !! start <= end
void dumpMemory(ShellContextPtr pContext, int argc, ...) {
  OFFSET start, end;

  va_list _vargs;
  va_start(_vargs, argc);
  // > dump or dump [start]
  if(argc <= 1) {
    start = argc == 0 ? pContext->memIdx : va_arg(_vargs, OFFSET);
    end = start + DEFAULT_BYTES - 1;
    if(end >= pContext->memSize) {
      end = pContext->memSize - 1;
    }
  }
  // > dump [start, end]
  else {
    start = va_arg(_vargs, OFFSET);
    end = va_arg(_vargs, OFFSET);
  }
  va_end(_vargs);

  pContext->memIdx = (end + 1) == pContext->memSize ? 0 : (end + 1);

  int rowStart = start/DUMP_COL, rowEnd = end/DUMP_COL;
  int r, c;
  OFFSET pos = start/DUMP_COL*DUMP_COL;

  for(r = rowStart; r <= rowEnd; ++r) {
    printf("%05X ", r * DUMP_COL);
    char asciiLine[DUMP_COL+1];
    asciiLine[DUMP_COL] = '\0';

    for(c = 0; c < DUMP_COL; ++c, ++pos) {
      if(pos < start || pos > end) {
        printf("   ");
        asciiLine[c] = '.';
        continue;
      }

      char byte = pContext->memAddr[pos];
      printf("%02X ", (unsigned)byte);
      asciiLine[c] =  isprint(byte) ? byte : '.';
    }

    printf("; %s\n", asciiLine);
  }
}

void editMemory(ShellContextPtr pContext, OFFSET pos, BYTE value) {
  pContext->memAddr[pos] = value;
}

void fillMemory(ShellContextPtr pContext, OFFSET start, OFFSET end, BYTE value) {
  memset(pContext->memAddr + start, value, end - start + 1);
}

void resetMemory(ShellContextPtr pContext) {
  memset(pContext->memAddr, 0, pContext->memSize);
}

void cleanupMemory(ShellContextPtr pContext) {
  free(pContext->memAddr);
  pContext->memSize = 0;
  pContext->memAddr = NULL;
  pContext->memIdx = 0;
}