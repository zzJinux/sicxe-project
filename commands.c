/** commands.c
 *  commands.h 구현부
 */
#include "commands.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
/* 명령처리에 쓰일 모듈들을 모두 불러옴 */
#include "./dir.h"
#include "./history.h"
#include "./memory.h"
#include "./opcode.h"
#include "./symtab.h"
#include "./util.h"

/* 미리 정의된 명령들의 레코드 */
static ShellCmd const SHELL_COMMANDS[] = {
  {"help", "h", "h[elp]", COMMAND_HELP},
  {"dir", "d", "d[ir]", COMMAND_DIR},
  {"quit", "q", "q[uit]", COMMAND_QUIT},
  {"history", "hi", "hi[story]", COMMAND_HISTORY},
  {"dump", "du", "du[mp] [start, end]", COMMAND_DUMP},
  {"edit", "e", "e[dit] address, value", COMMAND_EDIT},
  {"fill", "f", "f[ill] start, end, value", COMMAND_FILL},
  {"reset", NULL, "reset", COMMAND_RESET},
  {"opcode", NULL, "opcode mnemoic", COMMAND_OPCODE},
  {"opcodelist", NULL, "opcodelist", COMMAND_OPCODELIST},
  {"assemble", NULL, "assemble filename", COMMAND_ASSEMBLE},
  {"symbol", NULL, "symbol", COMMAND_SYMBOL},
  {"type", NULL, "type filename", COMMAND_TYPE}
};

/*
  다른 모듈들이 SHELL_COMMANDS의 길이를 링크 전에 알 수 없으므로
  별도의 함수 인터페이스를 정의함
*/
int getShellCmdCount() {
  return sizeof(SHELL_COMMANDS)/sizeof(ShellCmd);
}

/*
  마찬가지로 링크 이전에 SHELL_COMMANDS의 타입을 알 수 없어(고정길이 배열)
  extern 선언이 안되므로 별도의 함수 인터페이스를 통해
  포인터로 반환함
*/
ShellCmd const *getShellCmdList() {
  return SHELL_COMMANDS;
}

/** parseAddrPoint(내부함수)
 *  str을 hexadecimal offset 문자열로 보고 파싱,검사 후 *p에 저장
 * 
 *  @인수
 *    p - 계산결과 반환 포인터
 *    str - 파싱할 문자열
 *    BOUNDARY - offset의 upperbound (open)
 * 
 *  @반환
 *    수행 중 에러
 */
static EXIT_FLAG parseAddrPoint(OFFSET *p, char const* str, OFFSET BOUNDARY) {
  NUMBER_PARSE_ERROR f = parseHex_u20(str, p);
  // 파싱 실패
  if(f == INVALID_NUMBER_FORMAT) {
    return NOT_HEX;
  }
  else if(f == NUMBER_TOO_LONG) {
    return ARGUMENT_TOO_LONG;
  }
  // offset이 boundary를 넘어섰음
  if(*p >= BOUNDARY) {
    return OUT_OF_RANGE;
  }
  return 0;
}

/** parseAddrRange(내부함수)
 *  s_start, s_end 를 각각 offset range의 시작점, 끝점에 해당하는 offset의
 *  hexadecimal offset 문자열로 보고 파싱,검사 후 각각 *ps, *pe에 저장
 *  내부에서 다시 parseAddrPoint를 호출한다
 * 
 *  @인수
 *    ps - 시작점 계산결과 반환 포인터
 *    pe - 끝점 계산결과 반환 포인터
 *    s_start - 시작점에 대응되는 문자열
 *    s_end - 끝점에 대응되는 문자열
 *    BOUNDARY - offset의 upperbound (open)
 * 
 *  @반환
 *    수행 중 에러
 */
static EXIT_FLAG parseAddrRange(OFFSET *ps, OFFSET *pe, char const *s_start, char const *s_end, OFFSET BOUNDARY) {
  EXIT_FLAG f;
  f = parseAddrPoint(pe, s_end, BOUNDARY);
  // 끝점은 반드시 BOUNDARY 이내에 있어야 함
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  f = parseAddrPoint(ps, s_start, *pe+1);
  // 시작점은 반드시 s_end 보다 크지 말아야 함
  if(f & INVALID_ARGUMENT) {
    return f;
  }
  return 0;
}


/** parseByte(내부함수)
 *  str을 hexadecimal 문자열로 보고 파싱 후 *p에 저장
 *  값은 반드시 BYTE 범위에 있어야 함
 * 
 *  @인수
 *    p - 계산결과 반환 포인터
 *    str - 파싱할 문자열
 * 
 *  @반환
 *    수행 중 에러
 */
static EXIT_FLAG parseByte(BYTE *p, char const *str) {
  unsigned x;
  NUMBER_PARSE_ERROR f = parseHex_u20(str, &x);
  if(f == INVALID_NUMBER_FORMAT) {
    return NOT_HEX;
  }
  else if(f == NUMBER_TOO_LONG) {
    return ARGUMENT_TOO_LONG;
  }

  // 값이 BYTE 범위를 넘어서므로 에러로 처리
  if(x > 0xFF) {
    return OUT_OF_RANGE;
  }
  *p = (BYTE)x;
  return 0;
}

EXIT_FLAG COMMAND_HELP(ShellContextPtr pContext, const Arguments args) {
  // 단독 명령어임
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  // 미리 정의된 명령 리스트 출력
  int i, cnt = getShellCmdCount();
  for(i=0; i<cnt; ++i) {
    printf("%s\n", SHELL_COMMANDS[i].desc);
  }

  return 0;
}

EXIT_FLAG COMMAND_DIR(ShellContextPtr pContext, const Arguments args) {
  // 단독 명령어임
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  // 현재 디렉토리 출력
  printDir(".");
  return 0;
}

EXIT_FLAG COMMAND_QUIT(ShellContextPtr pContext, const Arguments args) {
  // 단독 명령어임
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  return QUIT_SHELL; // 쉘 종료 플래그
}

EXIT_FLAG COMMAND_HISTORY(ShellContextPtr pContext, const Arguments args) {
  // 단독 명령어임
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }
  // 출력 전에 자기자신에 대한 기록을 함
  updateHistory(pContext, args.RAW, args.RAW_LEN);
  printHistory(pContext);
  return 0;
}

/*
  메모리 공간과 관련된 명령어는 주소를 파싱하거나 값을 파싱하는 부분이 일치하여
    파싱 -> 에러시리턴 -> 본함수 호출
  의 유사한 구조를 가지고 있다.
  때문에 함수로 공통부분을 추출했으며 이에 대한 설명은 따로 하지 않는다
*/

EXIT_FLAG COMMAND_DUMP(ShellContextPtr pContext, const Arguments args) {
  // dump 명령어 이외에 최대 2개까지 허용됨
  if(args.argc > 3) {
    return UNKNOWN_ARGUMENT;
  }

  // dump(단독) 포맷
  if(args.argc == 1) {
    dumpMemory(pContext, 0);
  }
  // dump start 포맷
  else if(args.argc == 2) {
    OFFSET start;
    // 미리 정의한 함수로 파싱
    EXIT_FLAG f = parseAddrPoint(&start, args.argv[1], pContext->memSize);
    if(f & INVALID_ARGUMENT) {
      return f;
    }

    dumpMemory(pContext, 1, start);
  }
  // dump start,end 포맷
  else {
    OFFSET start, end;
    // 미리 정의한 함수로 파싱
    EXIT_FLAG f = parseAddrRange(&start, &end, args.argv[1], args.argv[2], pContext->memSize);
    if(f & INVALID_ARGUMENT) {
      return f;
    }

    dumpMemory(pContext, 2, start, end);
  }
  return 0;
}

EXIT_FLAG COMMAND_EDIT(ShellContextPtr pContext, const Arguments args) {
  // edit address,value 포맷임
  if(args.argc != 3) {
    return UNKNOWN_ARGUMENT;
  }

  EXIT_FLAG f;
  OFFSET pos;
  BYTE value;
    // 미리 정의한 함수로 파싱
  f = parseAddrPoint(&pos, args.argv[1], pContext->memSize);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

    // 미리 정의한 함수로 파싱
  f = parseByte(&value, args.argv[2]);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  editMemory(pContext, pos, value);
  return 0;
}

EXIT_FLAG COMMAND_FILL(ShellContextPtr pContext, const Arguments args) {
  // fill start,end,value 포맷임
  if(args.argc != 4) {
    return UNKNOWN_ARGUMENT;
  }

  EXIT_FLAG f;
  OFFSET start, end;
  BYTE value;
  // 미리 정의한 함수로 파싱
  f = parseAddrRange(&start, &end, args.argv[1], args.argv[2], pContext->memSize);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  // 미리 정의한 함수로 파싱
  f = parseByte(&value, args.argv[3]);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  fillMemory(pContext, start, end, value);
  return 0;
}

EXIT_FLAG COMMAND_RESET(ShellContextPtr pContext, const Arguments args) {
  // 단독 명령어임
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  resetMemory(pContext);
  return 0;
}

EXIT_FLAG COMMAND_OPCODE(ShellContextPtr pContext, const Arguments args) {
  // opcode mnemonic 포맷임
  if(args.argc != 2) {
    return UNKNOWN_ARGUMENT;
  }

  // mnemonic은 반드시 대문자여야함
  char *str = args.argv[1];
  while(isupper(*str)) ++str;
  if(*str != '\0') {
    printf("-note: mnemonic should be uppercase alphabet\n");
    return UNKNOWN_ARGUMENT;
  }

  // opcode 검색
  int opcode = findOpcode(pContext->opcodeTable, args.argv[1]);
  if(opcode != -1) {
    // 찾은 경우
    printf("opcode is %02X\n", opcode);
  }
  else {
    // 못찾은 경우
    printf("opcode is not defined for %s\n", str);
  }
  return 0;
} 

EXIT_FLAG COMMAND_OPCODELIST(ShellContextPtr pContext, const Arguments args) {
  // 단독 명령어임
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  printOpcodeList(pContext->opcodeTable);
  return 0;
}

EXIT_FLAG COMMAND_SYMBOL(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  int ret = printSymbolTable(pContext->symbolTable);
  if(ret == -1) {
    printf("-symbol: allocation fail while print\n");
    return INTERNAL_COMMAND_ERROR;
  }

  return 0;
}

EXIT_FLAG COMMAND_TYPE(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 2) {
    return UNKNOWN_ARGUMENT;
  }

  FILE *stream = fopen(args.argv[1], "r");
  if(stream == NULL) {
    printf("-type: error opening file\n");
    return INTERNAL_COMMAND_ERROR;
  }

  int ch;
  while((ch=fgetc(stream)) != EOF) {
    putchar(ch);
  }

  return 0;
}