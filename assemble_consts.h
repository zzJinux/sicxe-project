/** assemble_consts.h
 *  assemble에 사용되는 상수들을 정의
 */
#ifndef _ASSEMBLE_CONSTS_H_
#define _ASSEMBLE_CONSTS_H_

#define PROGRAM_NAME_LIMIT 6
#define LOC_MAX 0xffff

// WORD 관련 상수들
#define WORD_SIZE 3
#define WORD_MAX ((1<<(WORD_SIZE*8-1))-1)
#define WORD_MIN (-(1<<(WORD_SIZE*8-1)))

// .obj 코드의 text record의 최대 길이(byte)
#define TRECORD_CODE_LIMIT 30

// 각 directive의 이름
#define DIRECTIVE_START "START"
#define DIRECTIVE_END "END"
#define DIRECITVE_BYTE "BYTE"
#define DIRECTIVE_RESB "RESB"
#define DIRECITVE_WORD "WORD"
#define DIRECTIVE_RESW "RESW"
#define DIRECTIVE_BASE "BASE"
#define DIRECTIVE_NOBASE "NOBASE"

// pass2 에서 사용되는 flag 상수
enum _AssembleStateFlag {
  NO_OBJECT_CODE = 1<<0,
  BASE_AVAILABLE = 1<<1,
  FORMAT4_MODIFICATION_RECORD = 1<<2,
  PROCESS_COMPLETE = 1<<3,
  SKIP_LOC_PRINT = 1<<4,
  FLUSH_OBJRECORD = 1<<5
};

#endif