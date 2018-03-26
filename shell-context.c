/** shell-context.c
 * shell-context.h 구현
 */
#include "./shell-context.h"

#include <stdlib.h>
#include "./history.h"
#include "./memory.h"
#include "./opcode.h"

// 메모리 공간의 총 크기, 1u<<20 는 2^20, 즉 1 Mega
#define MEM_TOTAL (1u<<20)

// 동적할당 중에 문제 발생 시 NULL을 반환함
ShellContextPtr initShellContext(FILE *stream) {
  ShellContextPtr pContext = (ShellContextPtr)malloc(sizeof(ShellContext));
  if(pContext == NULL) {
    return NULL;
  }

  // 히스토리 목록 초기화
  initHistory(pContext);
  // 메모리 공간 할당
  BYTE *memStart = initMemory(pContext, MEM_TOTAL);
  if(memStart == NULL) {
    return NULL;
  }

  // stream 으로부터 opcode 정보를 읽어 해시테이블 초기화
  OpcodeHashNodePtr *pNodeList = initOpcodeList(pContext, stream);
  if(pNodeList == NULL) {
    return NULL;
  }

  return pContext;
}

void cleanupShellContext(ShellContextPtr pContext) {
  cleanupHistory(pContext);
  cleanupMemory(pContext);
  cleanupOpcodeList(pContext);
  free(pContext);
}
