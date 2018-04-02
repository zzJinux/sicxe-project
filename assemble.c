#include "./assemble.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./typedefs.h"
#include "./shell-context.h"
#include "./util.h"

#define PROGRAM_NAME_LIMIT 6

#define DIRECTIVE_START "START"
#define DIRECTIVE_END "END"
#define DIRECITVE_BYTE "BYTE"
#define DIRECTIVE_RESB "RESB"
#define DIRECITVE_WORD "WORD"
#define DIRECTIVE_RESW "RESW"
#define DIRECTIVE_BASE "BASE"
#define DIRECTIVE_NOBASE "NOBASE"

typedef enum _ASSEMBLE_ERROR {
  ADDR_RESOLUTION_FAIL = 1<<3,
  SYNTAX_PARSE_FAIL = 1<<4,
  ALLOC_FAIL = 1<<5
} ASSEMBLE_ERROR;

typedef enum _SYNTAX_ERROR {
  START_DIRECTIVE_WRONG_PLACE = 1,
  BAD_ADDRESS,
  PROGRAM_NAME_TOO_LONG,
  ADDR_TOO_LARGE
} SYNTAX_ERROR;

typedef struct _Token {
  char const *tokenText;
  int colNo;
} Token;

typedef struct _Statement {
  char const *rawText;
  unsigned lineNo;
  OFFSET loc;
  Token *label, *mnemonic, *operand;
} Statement;

static void printErrMsg(ASSEMBLE_ERROR code, char const* detail);
static void printSyntaxErrMsg(SYNTAX_ERROR code, char const* line, int lineNo, int colNo);

static Token *createToken(char const *str, int colNo, int *pLen);
static void cleanupToken(Token *pT);

ERROR_CODE assemble(ShellContextPtr pContext, FILE *asmIn, FILE *lstOut, FILE *objOut) {
  ASSEMBLE_ERROR errCode = 0;
  
  int lineNo = 0;
  OFFSET locBegin = 0;
  OFFSET locctr = 0;

  while(1) {
    Token *labelTok, *mnemoTok, *operandTok;
    labelTok = mnemoTok = operandTok = NULL;

    int tokenLen;
    char ch;

    char *line = freadLine(asmIn);
    if(line == NULL) {
      errCode = ALLOC_FAIL;
      printErrMsg(errCode, "");
      goto cleanup;
    }

    ++lineNo;

    int i=0;
    ch = jumpBlank(line, &i);
    if(ch == '\0' || ch == '.') {
      goto nextIteration;
    }

    if(i==0) {
      // label
      labelTok = createToken(line+i, i+1, &tokenLen);
      if(labelTok == NULL) {
        errCode = ALLOC_FAIL;
        printErrMsg(errCode, "");
        goto cleanup;
      }
      i += tokenLen;

      ch = jumpBlank(line, &i);
      if(ch == '\0') {
        goto nextIteration;
      }
    }

    // mnemonic
    mnemoTok = createToken(line+i, i+1, &tokenLen);
    if(mnemoTok == NULL) {
      errCode = ALLOC_FAIL;
      printErrMsg(errCode, "");
      goto cleanup;
    }
    i += tokenLen;

    ch = jumpBlank(line, &i);
    if(ch == '\0') {
      goto nextIteration;
    }

    // operand
    operandTok = createToken(line+i, i+1, &tokenLen);
    if(operandTok == NULL) {
      errCode = ALLOC_FAIL;
      printErrMsg(errCode, "");
      goto cleanup;
    }

    if(strcmp(mnemoTok->tokenText, DIRECTIVE_START) == 0) {
      if(locctr > 0) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(START_DIRECTIVE_WRONG_PLACE, line, lineNo, mnemoTok->colNo);
        goto cleanup;
      }

      // 프로그램 이름 길이 검사
      if(strlen(labelTok->tokenText) > PROGRAM_NAME_LIMIT) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(PROGRAM_NAME_TOO_LONG, line, lineNo, labelTok->colNo);
        goto cleanup;
      } 
 
      // TODO: parsing needs refinement
      // 프로그램 시작주소 검사
      sscanf(operandTok->tokenText, "%x", &locctr);
      locBegin = locctr;
      if(locctr >= pContext->memSize) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(BAD_ADDRESS, line, lineNo, operandTok->colNo);
        goto cleanup;
      }
    }

    if(strcmp(mnemoTok->tokenText, DIRECTIVE_RESB)) {
    }

  nextIteration:
    free(line);
    continue;
  cleanup:
    free(line);
    cleanupToken(labelTok);
    cleanupToken(mnemoTok);
    cleanupToken(operandTok);
    return  errCode;
  }

  return 0;

}

static void printErrMsg(ASSEMBLE_ERROR code, char const* detail) {
  if(code == 0) return;
  if(code & ADDR_RESOLUTION_FAIL) {
    printf("-assemble: address resolution fail\n");
  }
  else if(code & ALLOC_FAIL) {
    printf("-assemble: alloction fail\n");
  }
  else {
    printf("-assmemble: <unknown error>\n");
  }
}

static void printSyntaxErrMsg(SYNTAX_ERROR code, char const* line, int lineNo, int colNo) {
  printf("-assemble: syntax error, ");
  if(code == START_DIRECTIVE_WRONG_PLACE) {
    printf("START directive is at wrong place\n");
  }
  else {
    printf("<unknown error>\n");
  }
  int prefix = printf("->> %d: ", lineNo);
  printf("%s\n", line);
  printf("%*c", prefix+colNo, '^');
}

static Token *createToken(char const *str, int colNo, int *pLen) {
  char *tokenText = readToken(str, pLen);
  if(tokenText == NULL) {
    return NULL;
  }

  Token *pT = malloc(sizeof(Token));
  if(pT == NULL) {
    return NULL;
  }

  pT->tokenText = tokenText;
  pT->colNo = colNo;
  return pT;
}

static void cleanupToken(Token *pT) {
  free((void *)pT->tokenText);
  free(pT);
}