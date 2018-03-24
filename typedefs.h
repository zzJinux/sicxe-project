#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

typedef struct _Arguments {
  char const *RAW; int RAW_LEN;
  int argc; char **argv;
} Arguments;

typedef struct _HistNode {
  char *text;
  struct _HistNode *next;
} HistNode, *HistNodePtr;

typedef struct _ShellContext {
  HistNodePtr histRootNode;
  HistNodePtr histCurrentNode;
  unsigned memSize;
  char *memAddr;
  int memIdx;
} ShellContext, *ShellContextPtr;

#endif