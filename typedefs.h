#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

typedef struct _HistNode {
  char *text;
  struct _HistNode *next;
} HistNode, *HistNodePtr;

typedef unsigned OFFSET;
typedef unsigned char BYTE;

#endif