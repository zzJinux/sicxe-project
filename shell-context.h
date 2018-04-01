/** shell-context.h
 * 프로그램 상태을 저장하는 ShellContext 모듈 헤더
 */
#ifndef _SHELL_CONTEXT_H_
#define _SHELL_CONTEXT_H_

#include <stdio.h>
#include "./util.h"
#include "./typedefs.h"

/* ShellContext 구조체, 프로그램 상태를 저장함 */
typedef struct _ShellContext {
  HistNodePtr histRootNode; // history 리스트의 루트노드 포인터
  HistNodePtr histCurrentNode; // history 리스트에서 마지막으로 추가된 노드의 포인터

  unsigned memSize; // 메모리 공간의 총 크기
  BYTE *memAddr; // 메모리 공간을 가리키는 포인터
  OFFSET memIdx; // dump 커맨드에 의해 현재 가리키는 주소 offset

  // opcode 리스트를 저장하는 해시테이블
  HashTable *opcodeTable;
} ShellContext, *ShellContextPtr;

/** initShellContext
 *  프로그램 상태에 관한 구조체를 초기화, 생성함
 * 
 *  @인수
 *    stream - opcode 입력텍스트의 파일스트림
 *  
 *  @반환
 *    프로그램 상태를 저장한 ShellContext 구조체의 포인터, 오류 발생 시 NULL 반환
 */
ShellContextPtr initShellContext(FILE *stream);

/** cleanupShellContext
 *  initShellContext 및 프로그램 실행 중 할당받은 자원 해제
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void cleanupShellContext(ShellContextPtr pContext);

#endif