/** history.c
 *  history.h 구현
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./history.h"

void initHistory(ShellContextPtr pContext) {
  pContext->histCurrentNode = NULL;
  pContext->histRootNode = NULL;
}

// 동적할당 중에 문제 발생 시 NULL을 반환함
HistNodePtr updateHistory(ShellContextPtr pContext, char const* text, int len) {
  // node의 다음에 nextNode를 추가함
  HistNodePtr node, nextNode;
  node = pContext->histCurrentNode;
  nextNode = (HistNodePtr)malloc(sizeof(HistNode));

  // text 를 복사할 버퍼 동적할당
  char *buf = (char *)malloc((len+1) * sizeof(char));
  if(nextNode == NULL || buf == NULL) {
    return NULL;
  }

  // buf <- text 복사
  strncpy(buf, text, len);
  buf[len] = '\0';
  nextNode->text = buf;
  nextNode->next = NULL;

  // nextNode를 추가할 노드가 있으면 그대로 추가
  if(node != NULL) {
    node->next = nextNode;
  }
  // 그러한 노드가 없으면 새 노드를 root로 삼는다
  else {
    pContext->histRootNode = nextNode;
  }

  // 새로 추가된 노드를 마지막으로 추가된 노드로 저장하고 반환
  return pContext->histCurrentNode = nextNode;
}

void cleanupHistory(ShellContextPtr pContext) {
  HistNodePtr node = pContext->histRootNode;
  while(node != NULL) {
    // 할당 해제 전 다음 노드 저장
    HistNodePtr nextNode = node->next;
    free(node->text);
    free(node);

    node = nextNode;
  }
  // 할당 해제 후 무효화된 값을 덮어씌움
  pContext->histRootNode = NULL;
  pContext->histCurrentNode = NULL;
}

void printHistory(ShellContextPtr pContext) {
  int i;
  HistNodePtr node = pContext->histRootNode;
  // 출력할 node가 NULL일 때까지 출력
  for(i=1; node != NULL; ++i, node = node->next) {
    printf("%d\t%s", i, node->text);
  }
}