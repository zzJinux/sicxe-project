#ifdef _ASSEMBLE_SUB_IMPL_

static Token *createToken(char const *str, int colNo, int *pLen, ASSEMBLE_ERROR *pErr);
static void cleanupToken(Token *pT);

static Statement *createStatement(FILE *asmIn, int lineNo, int loc, ASSEMBLE_ERROR *pErr);
static Statement *dummyStatement(int loc);
static void cleanupStatement(Statement *st);

static ERROR_CODE assemble_pass1(FILE *asmIn, HashTable *optab, HashTable *symtab, Vec *stVec) {
  ASSEMBLE_ERROR errCode = 0;
  
  int lineNo = 1;
  int loc = 0;
  emptySymbolTable(symtab);

  while(!feof(asmIn)) {

    Statement *st = createStatement(asmIn, lineNo, loc, &errCode);
    if(st == NULL) goto cleanup;

    Token *tok;
    tok = st->mnemonic;

    if(tok == NULL) goto nextIteration;

    if(strcmp(tok->tokenText, DIRECTIVE_START) == 0) {
      if(lineNo > 1) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(START_DIRECTIVE_WRONG_PLACE, st, tok->colNo);
        goto cleanup;
      }

      // 프로그램 이름 길이 검사
      tok = st->label;
      if(tok != NULL && strlen(tok->tokenText) > PROGRAM_NAME_LIMIT) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(PROGRAM_NAME_TOO_LONG, st, tok->colNo);
        goto cleanup;
      } 
 
      tok = st->operand;
      if(tok == NULL) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(TOKEN_MISSING, st, 0);
        goto cleanup;
      }

      char *end;
      loc = strtol(tok->tokenText, &end, 16);
      if(*end != '\0' || loc < 0 || loc > LOC_MAX) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }

      st->loc = loc;

      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_RESB) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(TOKEN_MISSING, st, 0);
        goto cleanup;
      }

      char *end;
      int size = strtol(tok->tokenText, &end, 10);
      if(*end != '\0' || size <= 0 || size > LOC_MAX) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }

      loc += 1*size;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_RESW) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }

      char *end;
      int size = strtol(tok->tokenText, &end, 10);
      if(*end != '\0' || size <= 0 || 3*size > LOC_MAX) {
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
      if(tok == NULL) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(TOKEN_MISSING, st, 0);
        goto cleanup;
      }

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

        if(quotedLen-2 > TRECORD_CODE_LIMIT) {
          errCode = SYNTAX_PARSE_FAIL;
          printSyntaxErrMsg(CONSTANT_TOO_LARGE, st, tok->colNo);
          goto cleanup;
        }

        loc += quotedLen-2;
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

        if((quotedLen-2+1)/2 > TRECORD_CODE_LIMIT) {
          errCode = SYNTAX_PARSE_FAIL;
          printSyntaxErrMsg(CONSTANT_TOO_LARGE, st, tok->colNo);
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
      if(tok == NULL) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(TOKEN_MISSING, st, tok->colNo);
        goto cleanup;
      }

      char *end;
      int val = strtol(tok->tokenText, &end, 10);
      if(*end != '\0' || val < WORD_MIN || val > WORD_MAX) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(CONSTANT_TOO_LARGE, st, tok->colNo);
        goto cleanup;
      }

      loc += WORD_SIZE;
      goto nextIteration;
    }

    if(strcmp(tok->tokenText, DIRECTIVE_BASE) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, tok->colNo);
        goto cleanup;
      }

      goto nextIteration;
    }
    if(strcmp(tok->tokenText, DIRECTIVE_NOBASE) == 0) {
      goto nextIteration;
    }
    if(strcmp(tok->tokenText, DIRECTIVE_END) == 0) {
      tok = st->operand;
      if(tok == NULL) {
        goto _NO_EXPLICIT_START;
      }

      SymbolDef *sym = findSymbol(symtab, tok->tokenText);
      if(sym == NULL) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(UNDEFINED_SYMBOL, st, tok->colNo);
        goto cleanup;
      }

    _NO_EXPLICIT_START:
      st->loc = -1; goto nextIteration;
    }

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

      if(isdigit(labelName[0])) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(INVALID_SYMBOL_NAME, st, labelTok->colNo);
        goto cleanup;
      }

      if(findSymbol(symtab, labelName)) {
        errCode = SYNTAX_PARSE_FAIL;
        printSyntaxErrMsg(DUPLICATE_SYMBOL, st, labelTok->colNo);
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
  emptySymbolTable(symtab);
  return errCode;
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

static Statement *dummyStatement(int loc) {
  Statement *st = malloc(sizeof(Statement));
  if(st == NULL) {
    return NULL;
  }

  *st = (Statement){
    .line = NULL, .lineNo = 0, .loc = loc, .label = NULL, .mnemonic = NULL, .operand = NULL
  };

  return st;
}

static void cleanupStatement(Statement *st) {
  if(st == NULL) return;
  free((void *)st->line);
  cleanupToken(st->label);
  cleanupToken(st->mnemonic);
  cleanupToken(st->operand);
  free(st);
}

#endif