#include "./assemble.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./typedefs.h"
#include "./shell-context.h"
#include "./opcode.h"
#include "./symtab.h"
#include "./util.h"

#define PROGRAM_NAME_LIMIT 6
#define LOC_MAX 0xffff
#define WORD_SIZE 3
#define WORD_MAX ((1<<(WORD_SIZE*8-1))-1)
#define WORD_MIN (-(1<<(WORD_SIZE*8-1)))

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
  PROGRAM_NAME_TOO_LONG,
  LOC_EXCEEDED,
  INVALID_OPERAND_FORMAT,
  DUPLICATE_LABEL,
  INTEGER_OUT_OF_RANGE,
  UNDEFINED_OPCODE
} SYNTAX_ERROR;

static char const *SYNTAX_ERROR_MESSAGES[] = {
  "<no-error>",
  "START directive at wrong place",
  "PROGRAM_NAME too long",
  "location counter exceeded the limit",
  "invalid operand format",
  "label is already defined",
  "integer constant is out of range",
  "opcode undefined"
};

typedef struct _Token {
  char const *tokenText;
  int colNo;
} Token;

typedef struct _Statement {
  char const *line;
  int lineNo;
  int loc;
  Token *label, *mnemonic, *operand;
} Statement;

static void printErrMsg(ASSEMBLE_ERROR code, char const* detail);
static void printSyntaxErrMsg(SYNTAX_ERROR code, Statement *st, int colNo);

static Token *createToken(char const *str, int colNo, int *pLen, ASSEMBLE_ERROR *pErr);
static void cleanupToken(Token *pT);

static Statement *createStatement(FILE *asmIn, int lineNo, int loc, ASSEMBLE_ERROR *pErr);
static void cleanupStatement(Statement *st);

ERROR_CODE assemble(ShellContextPtr pContext, FILE *asmIn, FILE *lstOut, FILE *objOut) {
  ASSEMBLE_ERROR errCode = 0;
  
  int lineNo = 1;
  int locBegin = 0;
  int loc = 0;
  HashTable *symtab = pContext->symbolTable;
  emptySymbolTable(symtab);

  while(1) {

    Statement *st = createStatement(asmIn, lineNo, loc, &errCode);
    if(st == NULL) {
      printErrMsg(errCode, "");
      goto cleanup;
    }

    Token *tok;
    tok = st->mnemonic;

    if(tok == NULL) {
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_START) == 0) {
      if(loc > 0) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(START_DIRECTIVE_WRONG_PLACE, st, tok->colNo);
        goto cleanup;
      }

      // 프로그램 이름 길이 검사
      tok = st->label;
      if(strlen(tok->tokenText) > PROGRAM_NAME_LIMIT) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(PROGRAM_NAME_TOO_LONG, st, tok->colNo);
        goto cleanup;
      } 
 
      tok = st->operand;
      NUMBER_PARSE_ERROR f = parseHex_u20(tok->tokenText, (unsigned *)&loc);
      if(f == NUMBER_TOO_LONG || f == INVALID_NUMBER_FORMAT) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }

      st->loc = loc;
      locBegin = loc;

      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_RESB) == 0) {
      int size;

      tok = st->operand;
      sscanf(tok->tokenText, "%d", &size);
      if(size <= 0 || size > LOC_MAX) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }

      loc += 1*size;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_RESW) == 0) {
      int size;

      tok = st->operand;
      sscanf(tok->tokenText, "%d", &size);
      if(size <= 0 || size > LOC_MAX) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }

      loc += 3*size;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECITVE_BYTE) == 0) {
      // forms of C'<string>' or X'<hex>'
      tok = st->operand;
      char const *operandStr = tok->tokenText;
      char prefix = operandStr[0];

      ++operandStr;
      if(prefix == 'C') {
        if(operandStr[0] != '\'') {
          errCode = SYNTAX_PARSE_FAIL;
          printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo+1);
          goto cleanup;
        }
        int quotedLen = getTokenSize(operandStr);
        if(operandStr[quotedLen-1] != '\'' || quotedLen-2 <= 0) {
          errCode = SYNTAX_PARSE_FAIL;
          printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo+1+(quotedLen-1));
          goto cleanup;
        }

        loc += quotedLen - 2;
        goto nextIteration;
      }
      else if(prefix == 'X') {
        if(operandStr[0] != '\'') {
          errCode = SYNTAX_PARSE_FAIL;
          printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo+1);
          goto cleanup;
        }
        int quotedLen = getTokenSize(operandStr);
        if(operandStr[quotedLen-1] != '\'' || quotedLen-2 <= 0) {
          errCode = SYNTAX_PARSE_FAIL;
          printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo+1+(quotedLen-1));
          goto cleanup;
        }

        loc += (quotedLen-2+1)/2;
        goto nextIteration;
      }
      else {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }
    }

    if(strcmp(tok->tokenText, DIRECITVE_WORD) == 0) {
      tok = st->operand;
      char const *operandStr = tok->tokenText;
      int val;
      char chkCh;
      sscanf(operandStr, "%d%c", &val, &chkCh);
      if(!isspace(chkCh)) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }

      if(val < WORD_MIN || val > WORD_MAX) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INTEGER_OUT_OF_RANGE, st, tok->colNo);
        goto cleanup;
      }

      loc += WORD_SIZE;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_BASE) == 0) {
      st->loc = -1; goto nextIteration;
    }
    if(strcmp(tok->tokenText, DIRECTIVE_NOBASE) == 0) {
      st->loc = -1; goto nextIteration;
    }
    if(strcmp(tok->tokenText, DIRECTIVE_END) == 0) {
      st->loc = -1;
      goto cleanup;
    }

    HashTable *optab = pContext->opcodeTable;
    if(tok->tokenText[0]=='+') {
      OpcodeDef *opcodeDef = findOpcode(optab, tok->tokenText+1);
      if(opcodeDef == NULL || opcodeDef->fm != FORMAT34) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(UNDEFINED_OPCODE, st, tok->colNo);
        goto cleanup;
      }

      loc += 4;
    }
    else {
      OpcodeDef *opcodeDef = findOpcode(optab, tok->tokenText);
      if(opcodeDef == NULL) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(UNDEFINED_OPCODE, st, tok->colNo);
        goto cleanup;
      }

      loc += opcodeDef->fm;
    }

  nextIteration:
    if(loc > LOC_MAX) {
      errCode = SYNTAX_PARSE_FAIL;
      printSyntaxErrMsg(LOC_EXCEEDED, st, tok->colNo);
      goto cleanup;
    }
    
    Token *labelTok = st->label;
    if(labelTok) {
      char const *labelName = labelTok->tokenText;
      if(findSymbol(symtab, labelName)) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(DUPLICATE_LABEL, st, labelTok->colNo);
        goto cleanup;
      }

      addSymbol(symtab, labelName, st->loc);
    }
    ++lineNo;

    ////////
    if(st->loc != -1) printf("%04X\t%s\n", st->loc, st->line);
    else printf("    \t%s\n", st->line);
    cleanupStatement(st);
    continue;

  cleanup:
    cleanupStatement(st);
    return errCode;
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
  else if(code & SYNTAX_PARSE_FAIL) {
    // empty
  }
  else {
    printf("-assmemble: <unknown error>\n");
  }
}

static void printSyntaxErrMsg(SYNTAX_ERROR code, Statement *st, int colNo) {
  char const *line = st->line;
  int lineNo = st->lineNo;
  printf("-assemble: syntax error, ");
  if((int)code < sizeof(SYNTAX_ERROR_MESSAGES)/sizeof(char *)) {
    printf("%s\n", SYNTAX_ERROR_MESSAGES[code]);
  }
  else {
    printf("<unknown error>\n");
  }
  int prefix = printf("->> %d:%d ", lineNo, colNo);
  printf("%s\n", line);
  printf("%*c\n", prefix+colNo, '^');
}

static Token *createToken(char const *str, int colNo, int *pLen, ASSEMBLE_ERROR *pErr) {
  char *tokenText = readToken(str, pLen);
  if(tokenText == NULL) {
    if(*pLen == -1) { *pErr = SYNTAX_PARSE_FAIL; }
    else { *pErr = ALLOC_FAIL; }
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
  if(pT == NULL) return;
  free((void *)pT->tokenText);
  free(pT);
}

static Statement *createStatement(FILE *asmIn, int lineNo, int loc, ASSEMBLE_ERROR *pErr) {
  Statement *st = malloc(sizeof(Statement));
  char const *line = freadLine(asmIn);
  if(st == NULL || line == NULL) {
    *pErr= ALLOC_FAIL;
    goto cleanup;
  }

  *st = (Statement){
    .line = line, .lineNo = lineNo, .loc = loc, .label = NULL, .mnemonic = NULL, .operand = NULL
  };

  char ch;
  int i=0;
  ch = jumpBlank(line, &i);
  if(ch == '\0' || ch == '.') {
    st->loc = -1;
    return st;
  }

  Token *labelTok = NULL, *mnemoTok = NULL, *operandTok = NULL;
  int tokenLen;

  if(i==0) {
    // label
    labelTok = createToken(line+i, i+1, &tokenLen, pErr);
    if(labelTok == NULL) {
      goto cleanup;
    }
    i += tokenLen;
    st->label = labelTok;

    ch = jumpBlank(line, &i);
    // label-only statement
    if(ch == '\0') {
      st->lineNo = -1;
      return st;
    }
  }

  // mnemonic
  mnemoTok = createToken(line+i, i+1, &tokenLen, pErr);
  if(mnemoTok == NULL) {
    goto cleanup;
  }
  i += tokenLen;
  st->mnemonic = mnemoTok;

  ch = jumpBlank(line, &i);
  if(ch == '\0') {
    return st;
  }

  // operand
  operandTok = createToken(line+i, i+1, &tokenLen, pErr);
  if(operandTok == NULL) {
    goto cleanup;
  }
  st->operand = operandTok;

  return st;

cleanup:
  if(*pErr == SYNTAX_PARSE_FAIL) {
    printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, i+1);
  }
  cleanupStatement(st);
  return NULL;
}

static void cleanupStatement(Statement *st) {
  if(st == NULL) return;
  free((void *)st->line);
  cleanupToken(st->label);
  cleanupToken(st->mnemonic);
  cleanupToken(st->operand);
  free(st);
}