/** assemble_errs.h
 *  assemble 중 발생가능한 에러들에 대응되는 상수들을 정의
 */
#ifndef _ASSEMBLE_ERRS_H_
#define _ASSEMBLE_ERRS_H_

typedef enum _ASSEMBLE_ERROR {
  ADDR_RESOLUTION_FAIL = 1<<3, // 주소를 표현할 방법이 없는 경우
  SYNTAX_PARSE_FAIL = 1<<4, // 문법오류, 세부 내용은 _SYNTAX_ERROR 상수를 이용
  ALLOC_FAIL = 1<<5 // 동적할당 오류
} ASSEMBLE_ERROR;

typedef enum _SYNTAX_ERROR {
  START_DIRECTIVE_WRONG_PLACE = 1, // START 지시자가 첫 줄이 아닌 곳에 있음
  PROGRAM_NAME_TOO_LONG, // 프로그램 이름 길이가 너무 김
  LOC_EXCEEDED, // location counter가 허용범위를 넘어섬
  INVALID_OPERAND_FORMAT, // 잘못된 operand 형식
  INVALID_SYMBOL_NAME, // 잘못된 symbol 형식
  OPERAND_MISSING, // operand가 필요한데 없음
  DUPLICATE_SYMBOL, // 같은 symbol이 두 개 이상의 위치에서 정의됨
  UNDEFINED_OPCODE, // opcode table에 없는 mnemonic이 사용됨
  UNDEFINED_SYMBOL, // symbol table에 없는 symbol이 사용됨
  CONSTANT_TOO_LARGE, // 상수가 너무 큼
  TOKEN_MISSING, // oken이 필요한데 없음
  UNDEFINED_REGISTER // 정의에 없는 register 번호
} SYNTAX_ERROR;

/* enum _SYNTAX_ERROR에 정의된 상수들에 대응되는 에러메시지의 배열 */
char const *SYNTAX_ERROR_MESSAGES[] = {
  "<no-error>",
  "START directive at wrong place",
  "PROGRAM_NAME too long",
  "location counter exceeded the limit",
  "invalid operand format",
  "invalid symbol name",
  "operand is missing",
  "symbol is already defined",
  "opcode undefined",
  "symbol undefined",
  "constant is out-of-range",
  "token is missing",
  "register undefined"
};

#endif