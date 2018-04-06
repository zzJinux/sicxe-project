#include "./assemble_types.h"

#include <stdio.h>
#include <stdlib.h>
#include "./util.h"
#include "./assemble.h"
#include "./assemble_errs.h"

static typeof(&_assemble_printSyntaxErrMsg) printSyntaxErrMsg;

Token *createToken(char const *str, int colNo, int *pLen, ASSEMBLE_ERROR *pErr) {
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

void cleanupToken(Token *pT) {
  if(pT == NULL) return;
  free((void *)pT->tokenText);
  free(pT);
}

Statement *readStatement(FILE *asmIn, int lineNo, int loc, ASSEMBLE_ERROR *pErr) {
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

Statement *dummyStatement(int loc) {
  Statement *st = malloc(sizeof(Statement));
  if(st == NULL) {
    return NULL;
  }

  *st = (Statement){
    .line = NULL, .lineNo = 0, .loc = loc, .label = NULL, .mnemonic = NULL, .operand = NULL
  };

  return st;
}

void cleanupStatement(Statement *st) {
  if(st == NULL) return;
  free((void *)st->line);
  cleanupToken(st->label);
  cleanupToken(st->mnemonic);
  cleanupToken(st->operand);
  free(st);
}