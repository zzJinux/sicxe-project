#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

void initHistory(ShellContextPtr pContext) {
  pContext->histCurrentNode = NULL;
  pContext->histRootNode = NULL;
}

HistNodePtr updateHistory(ShellContextPtr pContext, char const* text, int len) {
  HistNodePtr node, nextNode;
  node = pContext->histCurrentNode;
  nextNode = (HistNodePtr)malloc(sizeof(HistNode));

  char *buf = (char *)malloc((len+1) * sizeof(char));
  if(nextNode == NULL || buf == NULL) {
    return NULL;
  }

  strncpy(buf, text, len);
  buf[len] = '\0';
  nextNode->text = buf;
  nextNode->next = NULL;

  if(node != NULL) {
    node->next = nextNode;
  }
  else {
    pContext->histRootNode = nextNode;
  }

  return pContext->histCurrentNode = nextNode;
}

void cleanupHistory(ShellContextPtr pContext) {
  HistNodePtr node = pContext->histRootNode;
  while(node != NULL) {
    HistNodePtr nextNode = node->next;
    free(node->text);
    free(node);

    node = nextNode;
  }
  pContext->histRootNode = NULL;
  pContext->histCurrentNode = NULL;
}

void printHistory(ShellContextPtr pContext) {
  int i;
  HistNodePtr node = pContext->histRootNode;
  for(i=1; node != NULL; ++i, node = node->next) {
    printf("%d\t%s", i, node->text);
  }
}