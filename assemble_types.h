#ifndef _ASSEMBLE_TYPES_H_
#define _ASSEMBLE_TYPES_H_

#include <stdio.h>
#include "./util.h"
#include "./typedefs.h"
#include "./assemble_errs.h"
#include "./assemble_consts.h"

typedef struct _Token {
  char const *tokenText; // Token의 문자열
  int colNo; // 줄 내에서의 토큰이 시작되는 열 번호
} Token;

typedef struct _Statement {
  char const *line; // Statement의 원래 입력
  int lineNo; // Statement의 줄번호
  int loc; // Statement에 대으되는 location coutner
  Token *label, *mnemonic, *operand; // Statement의 구성 토큰들
} Statement;

Token *createToken(char const *str, int colNo, int *pLen, ASSEMBLE_ERROR *pErr);
void cleanupToken(Token *pT);

Statement *readStatement(FILE *asmIn, int lineNo, int loc, ASSEMBLE_ERROR *pErr);
Statement *dummyStatement(int loc);
void cleanupStatement(Statement *st);

// Modification Record
typedef struct _MRec {
  OFFSET start;
  int halfBytes;
} MRec;

#define OBJCODE_BUF_LEN(baseT) ((TRECORD_CODE_LIMIT+sizeof(baseT)-1)/sizeof(baseT))

/* assemble pass2에 쓰이는 상태 구조체 */
typedef struct _AssembleState {
  char programName[PROGRAM_NAME_LIMIT+1];
  int programLength;
  int startAddr; // 프로그램의 시작 주소
  int baseAddr; // BASE directive로 지정되는 주소

  /** 
   * pcAddr은 instruction의 pc-relative 계산에 사용되거나
   * END Directive 가 지정한 시작 instruction의 주소를 저장함
   */
  int pcAddr;

  /* Text Record에 쓰일 obj code를 저장함 */
  unsigned objcodeBuf[OBJCODE_BUF_LEN(unsigned)];
  int objcodeLen; // 단위 obj code의 길이
  /* 파싱 도중에 발생하는 상태변화를 저장함*/
  enum _AssembleStateFlag flag;
  HashTable *optab, *symtab; // opcode table / symbol table

  /* Text Record 지연 출력을 위한 버퍼 */
  char textRecBuf[TRECORD_CODE_LIMIT*2+1];
  /* Text Record의 obj code byte 길이*/
  int curTextRecordLen;
  Vec *mRecVec; // modification record 저장용
} AssembleState;

#endif
