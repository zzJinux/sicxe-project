/**
 * 전역적으로 사용되는 typedef문을 모아놓은 헤더
 */
#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

typedef unsigned ERROR_CODE;
typedef unsigned OFFSET;
typedef unsigned char BYTE;

/* HistNode, history 의 LinkedList 노드 */
typedef struct _HistNode {
  char *text;
  struct _HistNode *next;
} HistNode, *HistNodePtr;

#endif