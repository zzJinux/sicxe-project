/** memory.c
 *  memory.h 구현
 */
#include "memory.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// dump 출력 시 한 줄에 출력되는 Byte 개수
#define DUMP_COL 16
// dumpMemory 포맷 1,2 에서 출력되는 Byte의 기본 개수
#define DEFAULT_BYTES 160

BYTE *initMemory(ShellContextPtr pContext, int memSize) {
  BYTE *memAddr = (BYTE *)malloc(memSize);
  if(memAddr == NULL) {
    return NULL;
  }

  // pContext의 메모리 공간 관련 멤버변수 초기화
  pContext->memSize = memSize;
  pContext->memAddr = memAddr;
  pContext->memIdx = 0;

  // 성공 시 메모리 공간의 주소를 반환
  return memAddr;
}

void dumpMemory(ShellContextPtr pContext, int argc, ...) {
  // 실제로 출력할 메모리 공간 범위의 시작점, 끝점 offset
  OFFSET start, end;

  // variadic arguments 관련 매크로(va_list, va_start, va_arg, va_end)
  // variadic arguments 처리 시작점
  va_list _vargs;
  va_start(_vargs, argc);

  // dumpMemory 포맷 1,2
  if(argc <= 1) {
    start = argc == 0 ? pContext->memIdx : va_arg(_vargs, OFFSET);
    end = start + DEFAULT_BYTES - 1;

    // DEFAULT_BYTES 기준으로 계산된 end가 메모리 공간을 벗어날 경우 조정해줌
    if(end >= pContext->memSize) {
      end = pContext->memSize - 1;
    }
  }
  // dumpMemory 포맷 3
  else {
    // 포맷 3는 start, end 가 지정된다. start <= end 를 상정함.
    start = va_arg(_vargs, OFFSET);
    end = va_arg(_vargs, OFFSET);
  }
  va_end(_vargs);
  // variadic arguments 처리 종료

  // 다음 dump 의 시작점을 계산
  pContext->memIdx = (end + 1) == pContext->memSize ? 0 : (end + 1);

  // [rowStart, rowEnd]는 출력할 메모리 공간을 DUMP_COL개의 열로 출력할 때
  // 출력되는 행번호의 범위임.
  int rowStart = start/DUMP_COL, rowEnd = end/DUMP_COL;

  // 행, 열 반복변수
  int r, c;
  // 출력되는 Byte의 offset
  OFFSET pos = start/DUMP_COL*DUMP_COL;

  for(r = rowStart; r <= rowEnd; ++r) {
    // r행의 첫 Byte 주소 출력
    printf("%05X ", r * DUMP_COL);

    // 뒤에 출력할 ascii 문자열의 버퍼
    char asciiLine[DUMP_COL+1];
    asciiLine[DUMP_COL] = '\0';

    // r행의 각 열에 해당하는 Byte 출력
    for(c = 0; c < DUMP_COL; ++c, ++pos) {
      // 출력할 위치가 아니면 공백으로 처리하고 ascii 문자열에는 dot 저장
      if(pos < start || pos > end) {
        printf("   ");
        asciiLine[c] = '.';
        continue;
      }

      // 출력할 Byte 저장
      BYTE byte = pContext->memAddr[pos];
      printf("%02X ", byte);

      // isprint로 프린트 가능한 문자인지 검사 후 저장한다
      asciiLine[c] =  isprint(byte) ? byte : '.';
    }

    // r행의 DUMP_COL 길이의 ascii 문자열 출력
    printf("; %s\n", asciiLine);
  }
}

// memory.h 참조
void editMemory(ShellContextPtr pContext, OFFSET pos, BYTE value) {
  pContext->memAddr[pos] = value;
}

// memory.h 참조
void fillMemory(ShellContextPtr pContext, OFFSET start, OFFSET end, BYTE value) {
  memset(pContext->memAddr + start, value, end - start + 1);
}

// memory.h 참조
void resetMemory(ShellContextPtr pContext) {
  memset(pContext->memAddr, 0, pContext->memSize);
}

void cleanupMemory(ShellContextPtr pContext) {
  free(pContext->memAddr);
  // 할당해제 후 무효화된 값을 덮어씌움
  pContext->memSize = 0;
  pContext->memAddr = NULL;
  pContext->memIdx = 0;
}