/** memory.h
 * memory 관리 모듈 헤더
 */
#ifndef _SIM_MEMORY_H_
#define _SIM_MEMORY_H_

#include "shell-context.h"

/** initMemory
 *  메모리 공간 생성/할당 함수
 * 
 *  @인수
 *    pContext - 프로그램 상태
 *    memSize - 메모리 공간의 총 크기
 *  
 *  @반환
 *    할당된 메모리 공간의 포인터, 실패 시 NULL
 */
BYTE *initMemory(ShellContextPtr pContext, int memSize);

/** dumpMemory
 *  메모리 공간을 dump 해주는 함수. variadic function 으로, 3가지 호출 포맷이 있음
 * 
 *  (1) dumpMemory(pContext, 0)
 *    첫 호출 시 0x00000, 그 외에는 마지막 dump한 영역의 뒤부터 정해진 수만큼의 byte를 dump함
 *    만약 메모리 공간의 끝에 도달해 지정 개수만큼 출력하지 못하면 0xFFFFF 까지만 출력하고
 *    그 뒤부턴 0x00000 로 다시 돌아옴
 *  (2) dumpMemory(pContext, 1, start)
 *    (1) 과 비슷하나 명시된 start 위치부터 메모리 공간 dump
 *  (3) dumpMemory(pContext, 2, start, end)
 *    [start, end] 범위를 dump함.
 * 
 *  start와 end는 항상 메모리 공간의 유효한 위치의 offset을 가지고 있어야하며
 *  반드시 start <= end 여야 함. 함수 구현은 항상 이러한 조건을 상정한다.
 * 
 *  @인수
 *    pContext - 프로그램 상태
 *    argc - 호출 포맷 지정용
 *    [OFFSET start] - dump할 공간의 시작점 offset (포맷 2,3)
 *    [OFFSET end] - dump할 공간의 끝점 offset (포맷 3)
 */
void dumpMemory(ShellContextPtr pContext, int argc, ...);

/** editMemory
 *  메모리 공간의 특정 위치(pos)의 값을 value로 수정함
 * 
 *  @인수
 *    pContext - 프로그램 상태
 *    pos - 수정할 위치의 offset
 *    value - 수정 후 Byte값
 */
void editMemory(ShellContextPtr pContext, OFFSET pos, BYTE value);

/** fillMemory
 *  메모리 공간의 범위([start, end])의 모든 값을 valueㄹ 수정함
 *  start, end는 dumpMemory(포맷 3) 과 동일한 조건을 만족한다고 상정
 * 
 *  @인수
 *    pContext - 프로그램 상태
 *    start - 수정할 공간의 시작점 offset
 *    end - 수정할 공간의 끝점 offset
 *    value - 수정 후 Byte값
 */
void fillMemory(ShellContextPtr pContext, OFFSET start, OFFSET end, BYTE value);

/** restMemory
 *  메모리 공간 전체를 0x00 으로 초기화
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void resetMemory(ShellContextPtr pContext);

/** cleanupMemory
 *  할당된 메모리 공간 해제
 * 
 *  @인수
 *    pContext - 프로그램 상태
 */
void cleanupMemory(ShellContextPtr pContext);

#endif