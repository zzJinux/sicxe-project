#include "./assemble.h"

#include <string.h>
#include <stdlib.h>
#include "./opcode.h"
#include "./symtab.h"
#include "./assemble_matchers.h"

static typeof(&_assemble_printSyntaxErrMsg) printSyntaxErrMsg = _assemble_printSyntaxErrMsg;

ASSEMBLE_ERROR assemble_pass1(FILE *asmIn, HashTable *optab, HashTable *symtab, Vec *stVec) {
  ASSEMBLE_ERROR errCode = 0;
  SYNTAX_ERROR syntaxErrCode = 0;
  
  Statement *st = NULL;
  int colNo = 0;
  int lineNo = 1;
  int loc = 0;
  emptySymbolTable(symtab);

  while(!feof(asmIn)) {

    st = readStatement(asmIn, lineNo, loc, &errCode);
    if(st == NULL) goto cleanup;

    Token *tok;
    tok = st->mnemonic;

    if(tok == NULL) goto nextIteration;

    if(strcmp(tok->tokenText, DIRECTIVE_START) == 0) {
      if(lineNo > 1) {
        syntaxErrCode = START_DIRECTIVE_WRONG_PLACE;
        colNo = tok->colNo;
        goto cleanup;
      }

      // 프로그램 이름 길이 검사
      tok = st->label;
      if(tok != NULL && strlen(tok->tokenText) > PROGRAM_NAME_LIMIT) {
        syntaxErrCode = PROGRAM_NAME_TOO_LONG;
        colNo = tok->colNo;
        goto cleanup;
      } 
 
      tok = st->operand;
      if(tok == NULL) {
        syntaxErrCode = OPERAND_MISSING;
        colNo = 0;
        goto cleanup;
      }

      char *end;
      loc = strtol(tok->tokenText, &end, 16);
      if(*end != '\0' || loc < 0 || loc > LOC_MAX) {
        syntaxErrCode = INVALID_OPERAND_FORMAT;
        colNo = tok->colNo;
        goto cleanup;
      }

      st->loc = loc;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_RESB) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        syntaxErrCode = OPERAND_MISSING;
        colNo = 0;
        goto cleanup;
      }

      char *end;
      int size = strtol(tok->tokenText, &end, 10);
      if(*end != '\0' || size <= 0 || size > LOC_MAX) {
        syntaxErrCode = LOC_OUT_OF_RANGE;
        colNo = tok->colNo;
        goto cleanup;
      }

      loc += 1*size;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_RESW) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        syntaxErrCode = OPERAND_MISSING;
        colNo = 0;
        goto cleanup;
      }

      char *end;
      int size = strtol(tok->tokenText, &end, 10);
      if(*end != '\0' || size <= 0 || 3*size > LOC_MAX) {
        syntaxErrCode = LOC_OUT_OF_RANGE;
        colNo = tok->colNo;
        goto cleanup;
      }

      loc += 3*size;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_BYTE) == 0) {
      // forms of C'<string>' or X'<hex>'
      tok = st->operand;
      if(tok == NULL) {
        syntaxErrCode = OPERAND_MISSING;
        colNo = 0;
        goto cleanup;
      }

      char const *literal = tok->tokenText;
      int len, byteLen;
      if(literal[0] == 'C') {
        if(!charLiteralMatcher(literal, &len)) {
          syntaxErrCode = INVALID_OPERAND_FORMAT;
          colNo = len;
          goto cleanup;
        }

        byteLen = len-3;
        if(byteLen > TRECORD_CODE_LIMIT) {
          syntaxErrCode = CONSTANT_TOO_LARGE;
          colNo = tok->colNo;
          goto cleanup;
        }

        loc += byteLen;
        goto nextIteration;
      }
      else if(literal[0] == 'X') {
        if(!hexLiteralMatcher(literal, &len)) {
          syntaxErrCode = INVALID_OPERAND_FORMAT;
          colNo = len;
          goto cleanup;
        }

        byteLen = (len-3+1)/2;
        if(byteLen > TRECORD_CODE_LIMIT) {
          syntaxErrCode = CONSTANT_TOO_LARGE;
          colNo = tok->colNo;
          goto cleanup;
        }

        loc += byteLen;
        goto nextIteration;
      }
      else {
        syntaxErrCode = INVALID_OPERAND_FORMAT;
        colNo = tok->colNo;
        goto cleanup;
      }
    }

    if(strcmp(tok->tokenText, DIRECTIVE_WORD) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        syntaxErrCode = OPERAND_MISSING;
        colNo = 0;
        goto cleanup;
      }

      char *end;
      int val = strtol(tok->tokenText, &end, 10);
      if(*end != '\0' || val < WORD_MIN || val > WORD_MAX) {
        syntaxErrCode = CONSTANT_TOO_LARGE;
        colNo = tok->colNo;
        goto cleanup;
      }

      loc += WORD_SIZE;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_BASE) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        syntaxErrCode = INVALID_OPERAND_FORMAT;
        colNo = 0;
        goto cleanup;
      }

      goto nextIteration;
    }
    if(strcmp(tok->tokenText, DIRECTIVE_NOBASE) == 0) {
      if(st->operand != NULL) {
        syntaxErrCode = INVALID_OPERAND_FORMAT;
        colNo = tok->colNo;
        goto cleanup;
      }
      goto nextIteration;
    }
    if(strcmp(tok->tokenText, DIRECTIVE_END) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        goto _NO_EXPLICIT_START;
      }

      SymbolDef *sym = findSymbol(symtab, tok->tokenText);
      if(sym == NULL) {
        syntaxErrCode = UNDEFINED_SYMBOL;
        colNo = tok->colNo;
        goto cleanup;
      }

    _NO_EXPLICIT_START:
      st->loc = -1; goto nextIteration;
    }

    if(tok->tokenText[0]=='+') {
      OpcodeDef *opcodeDef = findOpcode(optab, tok->tokenText+1);
      if(opcodeDef == NULL || opcodeDef->fm != FORMAT34) {
        syntaxErrCode = UNDEFINED_OPCODE;
        colNo = tok->colNo;
        goto cleanup;
      }

      loc += 4;
    }
    else {
      OpcodeDef *opcodeDef = findOpcode(optab, tok->tokenText);
      if(opcodeDef == NULL) {
        syntaxErrCode = UNDEFINED_OPCODE;
        colNo = tok->colNo;
        goto cleanup;
      }

      loc += opcodeDef->fm;
    }

  nextIteration:
    if(loc > LOC_MAX) {
      syntaxErrCode = LOC_OUT_OF_RANGE;
      colNo = 0;
      goto cleanup;
    }
    
    Token *labelTok = st->label;
    if(
      labelTok &&
      !(st->mnemonic && strcmp(st->mnemonic->tokenText, DIRECTIVE_START) == 0)
    ) {
      char const *labelName = labelTok->tokenText;

      if(!identifierMatcher(labelName, NULL)) {
        syntaxErrCode = INVALID_SYMBOL_NAME;
        colNo = labelTok->colNo;
        goto cleanup;
      }

      if(findSymbol(symtab, labelName)) {
        syntaxErrCode = DUPLICATE_SYMBOL;
        colNo = labelTok->colNo;
        goto cleanup;
      }

      if(addSymbol(symtab, labelName, st->loc) == NULL) {
        errCode = ALLOC_FAIL;
        goto cleanup;
      }
    }

    if(vecPush(stVec, st) == NULL) {
      errCode = ALLOC_FAIL;
      goto cleanup;
    }

    if(st->loc == -1) {
      break;
    }
    
    ++lineNo;
    continue;

  cleanup:
    goto _cleanup_;
  }

  Statement *dummy = dummyStatement(loc);
  if(dummy == NULL || vecPush(stVec, dummy) == NULL) {
    errCode = ALLOC_FAIL;
    goto _cleanup_;
  }

  return 0;

_cleanup_:
  if(syntaxErrCode) {
    errCode = SYNTAX_PARSE_FAIL;
    printSyntaxErrMsg(syntaxErrCode, st, colNo);
  }
  emptySymbolTable(symtab);
  return errCode;
}
