#include "./assemble.h"

#include <string.h>
#include <stdlib.h>
#include "./opcode.h"
#include "./symtab.h"

#define REGS_SIZE (sizeof(REGISTERS)/sizeof(REGISTERS[0]))

static typeof(&_assemble_printSyntaxErrMsg) printSyntaxErrMsg;

static struct _Registers {
  char mnemonic[3];
  BYTE number;
} const REGISTERS[] = {
  {"A", 0}, {"X", 1}, {"L", 2}, {"B", 3}, {"S", 4}, {"T", 5}, {"F", 6},
  {"PC", 8}, {"SW", 9}
};

static int findRegisterNumber(char const *reg, int len) {
  int i;
  for(i=0; i<REGS_SIZE; ++i) {
    if(strncmp(REGISTERS[i].mnemonic, reg, len) == 0) {
      return REGISTERS[i].number;
    }
  }
  return -1;
}

static int initStates(AssembleState *pState, HashTable *optab, HashTable *symtab, Vec *stVec);

static ASSEMBLE_ERROR processDirective(AssembleState *pState, Statement *st);
static ASSEMBLE_ERROR processOpcode(AssembleState *pState, Statement *st);

static void printLst(FILE *lstOut, AssembleState *pState, Statement *st);

static void printHeaderRec(FILE *objOut, AssembleState *pState);
static int printTextRec(FILE *objOut, AssembleState *pState, OFFSET loc);
static void printModRec(FILE *objOut, AssembleState *pState);
static void printEndRec(FILE *objOut, AssembleState *pState);

ASSEMBLE_ERROR assemble_pass2(FILE *lstOut, FILE *objOut, HashTable *optab, HashTable *symtab, Vec *stVec) {
  ASSEMBLE_ERROR asmErr;

  AssembleState state;

  Statement **stArr = (Statement **)stVec->arr;

  // START 지시자는 없거나 반드시 첫 줄에 등장한다고 가정
  int stIdx, size = stVec->size;
  int ret = initStates(&state, optab, symtab, stVec);
  if(ret) {
    asmErr = ALLOC_FAIL;
    goto cleanup;
  }

  printHeaderRec(objOut, &state);

  // omit last dummy statement
  for(stIdx = 0; stIdx < size-1; ++stIdx) {
    Statement *st = stArr[stIdx];
    state.flag &= BASE_AVAILABLE;

    asmErr = processDirective(&state, st);
    if(asmErr) {
      goto cleanup;
    }

    if(!(state.flag & PROCESS_COMPLETE)) {
      state.pcAddr = stArr[stIdx+1]->loc;
    }
    asmErr = processOpcode(&state, st);
    if(asmErr) {
      goto cleanup;
    }

    printTextRec(objOut, &state, st->loc);

    printLst(lstOut, &state, st);
  }

  printModRec(objOut, &state);
  printEndRec(objOut, &state);

  return 0;

cleanup:
  cleanupVec(state.mRecVec);
  return asmErr;
}

static int initStates(AssembleState *pState, HashTable *optab, HashTable *symtab, Vec *stVec) {
  Statement *first, *last;
  first = ((Statement **)stVec->arr)[0];
  last = ((Statement **)stVec->arr)[stVec->size-1];

  pState->programName[0] = '\0';

  Token *s = first->mnemonic;
  if(s != NULL && strcmp(s->tokenText, DIRECTIVE_START) == 0 && first->label != NULL) {
    strcpy(pState->programName, first->label->tokenText);
  }
  
  pState->programLength = last->loc - first->loc;
  pState->startAddr = first->loc;
  pState->pcAddr = first->loc;
  pState->objcodeLen = 0;
  pState->flag = 0;

  pState->optab = optab;
  pState->symtab = symtab;

  pState->curTextRecordLen = 0;
  pState->mRecVec = initVec(4, free);
  if(pState->mRecVec == NULL) {
    return -1;
  }

  return 0;
}

static ASSEMBLE_ERROR processDirective(AssembleState *pState, Statement *st) {
  if(st->mnemonic == NULL) {
    if(st->label == NULL) {
      pState->flag |= SKIP_LOC_PRINT;
    }
    pState->flag |= NO_OBJECT_CODE | PROCESS_COMPLETE;
    return 0;
  }

  HashTable *symtab = pState->symtab;
  Token *directive = st->mnemonic, *operand = st->operand;

  char const *tokText = directive->tokenText;
  if(strcmp(tokText, DIRECTIVE_RESB) == 0) {
    pState->flag |= FLUSH_OBJRECORD | NO_OBJECT_CODE;
  }
  else if(strcmp(tokText, DIRECTIVE_RESW) == 0) {
    pState->flag |= FLUSH_OBJRECORD | NO_OBJECT_CODE;
  }
  else if(strcmp(tokText, DIRECTIVE_BYTE) == 0) {
    char const *str = operand->tokenText;
    unsigned *buf = pState->objcodeBuf;
    int const SZ = sizeof(unsigned);

    int len, i;
    if(str[0] == 'C') {
      str += 2;
      len = pState->objcodeLen = strlen(str)-1;
      for(i=0; i<len; ++i) {
        buf[i/SZ] = buf[i/SZ]<<8 | str[i];
      }
    }
    else /* if str[0] == 'X' */ {
      str += 2;
      len = strlen(str)-1;
      pState->objcodeLen = (len+1)/2;
      char t[2]; t[1]=0;
      for(i=0; i<len; ++i) {
        t[0] = str[i];
        buf[i/2/SZ] = buf[i/2/SZ]<<4 | strtol(t, NULL, 16);
      }
    }
  }
  else if(strcmp(tokText, DIRECTIVE_WORD) == 0) {
    pState->objcodeBuf[0] = strtol(operand->tokenText, NULL, 10);
    pState->objcodeLen = 3;
  }
  else if(strcmp(tokText, DIRECTIVE_BASE) == 0) {
    SymbolDef *sym = findSymbol(symtab, operand->tokenText);
    if(sym == NULL) {
      printSyntaxErrMsg(UNDEFINED_SYMBOL, st, operand->colNo);
      return SYNTAX_PARSE_FAIL;
    }

    pState->baseAddr = sym->loc;
    pState->flag |= BASE_AVAILABLE | NO_OBJECT_CODE | SKIP_LOC_PRINT;
  }
  else if(strcmp(tokText, DIRECTIVE_NOBASE) == 0) {
    pState->flag &= ~BASE_AVAILABLE;
    pState->flag |= NO_OBJECT_CODE | SKIP_LOC_PRINT;
  }
  else if(strcmp(tokText, DIRECTIVE_START) == 0) {
    pState->flag |= NO_OBJECT_CODE;
  }
  else if(strcmp(tokText, DIRECTIVE_END) == 0) {
    if(operand != NULL) {
      SymbolDef *sym = findSymbol(symtab, operand->tokenText);
      if(sym == NULL) {
        printSyntaxErrMsg(UNDEFINED_SYMBOL, st, operand->colNo);
        return SYNTAX_PARSE_FAIL;
      }
      pState->pcAddr = sym->loc;
    }
    else {
      pState->pcAddr = pState->startAddr;
    }

    pState->flag |= FLUSH_OBJRECORD | NO_OBJECT_CODE | SKIP_LOC_PRINT;
  }
  else {
    pState->flag &= ~PROCESS_COMPLETE;
    return 0;
  }

  pState->flag |= PROCESS_COMPLETE;

  return 0;
}

static void printLst(FILE *lstOut, AssembleState *pState, Statement *st) {
  fprintf(lstOut, "%-4d ", st->lineNo*5);
  if(pState->flag & SKIP_LOC_PRINT) {
    fprintf(lstOut, "      ");
  }
  else {
    fprintf(lstOut, "%04X  ", st->loc);
  }
  int cnt = fprintf(lstOut, "%s", st->line);
  for(cnt=48-cnt; cnt--; ) fputc(' ', lstOut);
  if(!(pState->flag & NO_OBJECT_CODE)) {
    unsigned *buf = pState->objcodeBuf;
    int numBytes = pState->objcodeLen;
    int const SZ = sizeof(unsigned);

    int i;
    for(i=0; i<numBytes/SZ; ++i) {
      fprintf(lstOut, "%0*X", SZ*2, buf[i]);
    }
    if(numBytes%SZ) {
      int r = numBytes%SZ;
      unsigned mask = ~(~0u<<(r*8));
      fprintf(lstOut, "%0*X", r*2, buf[i]&mask);
    }
  }
  fputc('\n', lstOut);
}

static ASSEMBLE_ERROR processOpcode(AssembleState *pState, Statement *st) {
  if(pState->flag & PROCESS_COMPLETE) {
    return 0;
  }

  enum _INSTRUCTION_FORMAT_FLAG iff = DEFAULT_FORMAT_HANDLING;

  unsigned instruction;
  char *mnemoName, *oprndText;
  mnemoName = (char *)st->mnemonic->tokenText;
  oprndText = st->operand ? (char *)st->operand->tokenText : NULL;

  int extended = mnemoName[0] == '+';

  OpcodeDef *opd = findOpcode(pState->optab, mnemoName + extended);
  OPCODE_FORMAT fm = opd->fm;

  iff = INSTRUCTION_FORMAT_DETECTOR(opd);

  if(fm != FORMAT1 && iff != FORMAT34_NO_ARG && oprndText == NULL) {
    printSyntaxErrMsg(OPERAND_MISSING, st, 0);
    return SYNTAX_PARSE_FAIL;
  }

  // 밀고
  instruction = opd->hex << 24;
  instruction |= extended << 20;

  if(fm == FORMAT34) {
    int i=0;
    int disp=0;
    BYTE mode = 3<<3;

    if(iff == FORMAT34_NO_ARG) {
      if(oprndText != NULL) {
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo);
        return SYNTAX_PARSE_FAIL;
      }
      goto l_FORMAT34_ASSIGN;
    }
    
    if(oprndText[i] == '#') {
      ++i;
      mode = 1<<3;
    }
    else if(oprndText[i] == '@') {
      ++i;
      mode = 2<<3;
    }

    int tLen, nextIdx;
    nextIdx = findToken(oprndText+i, ',', NULL, &tLen);
    if(nextIdx == -1) {
      printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
      return SYNTAX_PARSE_FAIL;
    }

    if(isdigit(oprndText[i])) {
      char *end;
      disp = strtol(oprndText+i, &end, 10);
      if(*end != '\0' &&!isspace(*end) && *end != ',') {
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }

      if(extended && (disp >= -(1<<19) || disp < (1<<19))) {}
      else if(disp >= -(1<<11) && disp < (1<<11)) {
        disp <<= 8;
      }
      else {
        printSyntaxErrMsg(CONSTANT_TOO_LARGE, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }
    }
    else {
      char tmp = oprndText[i+tLen];
      oprndText[i+tLen] = '\0';
      SymbolDef *sym = findSymbol(pState->symtab, oprndText+i);
      oprndText[i+tLen] = tmp;

      if(sym == NULL) {
        printSyntaxErrMsg(UNDEFINED_SYMBOL, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }

      int loc = sym->loc;
      disp = loc - pState->pcAddr;
      if(disp >= -(1<<11) && disp < (1<<11)) {
        mode |= 1;
      }
      else if(pState->flag & BASE_AVAILABLE) {
        disp = loc - pState->baseAddr;
        if(disp >= 0 && disp < (1<<12)) {
          mode |= 2;
        }
      }

      if(extended && !(mode & 3)) {
        disp = loc;
        pState->flag |= FORMAT4_MODIFICATION_RECORD;
      }
      else if(mode & 3) {
        disp &= 0xfff;
        disp <<= 8;
      }
      else {
        return ADDR_RESOLUTION_FAIL;
      }
    }

    if(nextIdx > 0) {
      i += nextIdx;
      nextIdx = findToken(oprndText+i, ',', NULL, &tLen);

      if(nextIdx != 0) {
        if(nextIdx == -1) i+=tLen;
        else i+=nextIdx;
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }

      if(strncmp(oprndText+i, "X", tLen) != 0) {
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }

      mode |= 4;
    }

  l_FORMAT34_ASSIGN:
    instruction |= (mode << 21) | disp;
  }

  if(fm == FORMAT2) {
    int i=0, nextIdx, tLen, regN;
    nextIdx = findToken(oprndText+i, ',', NULL, &tLen);
    if(nextIdx == -1) {
      i+=tLen;
      printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
      return SYNTAX_PARSE_FAIL;
    }

    if(iff != FORMAT2_CONSTANT_ARG) {
      regN = findRegisterNumber(oprndText, tLen);
      if(regN == -1) {
        printSyntaxErrMsg(UNDEFINED_REGISTER, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }
    }
    else {
      char *end;
      regN = strtol(oprndText, &end, 10);
      if(*end != '\0' && !isspace(*end) && *end != ',') {
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }
      if(regN < 0 || regN >= 1<<4) {
        printSyntaxErrMsg(CONSTANT_TOO_LARGE, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }
    }

    instruction |= regN<<20;

    if(nextIdx == 0) {
      if(iff == FORMAT2_CONSTANT_ARG || iff == FORMAT2_SINGLE_ARG) {
        goto final;
      }

      printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
      return SYNTAX_PARSE_FAIL;
    }

    i+=nextIdx;
    nextIdx = findToken(oprndText+i, ',', NULL, &tLen);
    if(nextIdx != 0) {
      if(nextIdx == -1) i+=tLen;
      else i+=nextIdx;
      printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
      return SYNTAX_PARSE_FAIL;
    }

    if(iff != FORMAT2_SHIFT_OP) {
      regN = findRegisterNumber(oprndText+i, tLen);
      if(regN == -1) {
        printSyntaxErrMsg(UNDEFINED_REGISTER, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }
    }
    else {
      char *end;
      regN = strtol(oprndText, &end, 10);
      if(*end != '\0' && !isspace(*end) && *end != ',') {
        printSyntaxErrMsg(INVALID_OPERAND_FORMAT, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }
      if(regN < 0 || regN >= 1<<4) {
        printSyntaxErrMsg(CONSTANT_TOO_LARGE, st, st->operand->colNo+i);
        return SYNTAX_PARSE_FAIL;
      }
    }

    instruction |= regN<<16;
  }
  
final:
  // 당기기
  instruction >>= ((4-(int)fm-extended) * 8);
  pState->objcodeBuf[0] = instruction;
  pState->objcodeLen = (int)fm + extended;

  pState->flag |= PROCESS_COMPLETE;
  return 0;
}

static void printHeaderRec(FILE *objOut, AssembleState *pState) {
  fputc('H', objOut);
  fprintf(objOut, "%-*s%06X%06X\n",
    PROGRAM_NAME_LIMIT,
    pState->programName,
    pState->pcAddr,
    pState->programLength
  );
}

static int printTextRec(FILE *objOut, AssembleState *pState, OFFSET loc) {
  int curLen = pState->curTextRecordLen;
  if(curLen > 0 && (
      pState->flag & FLUSH_OBJRECORD ||
      curLen + pState->objcodeLen > TRECORD_CODE_LIMIT
  )) {
    fprintf(objOut, "%02X%.*s\n", curLen, curLen*2, pState->textRecBuf);
    curLen = 0;
  }

  if(pState->flag & NO_OBJECT_CODE) {
    pState->curTextRecordLen = curLen;
    return 0;
  }

  if(curLen == 0) {
    fprintf(objOut, "T%06X", loc);
  }

  if(pState->flag & FORMAT4_MODIFICATION_RECORD) {
    Vec *vec = pState->mRecVec;
    MRec *mRec = malloc(sizeof(MRec));
    if(mRec == NULL) {
      return -1;
    }

    mRec->start = loc+1;
    mRec->halfBytes = 5;

    if(vecPush(vec, mRec) == NULL) {
      return -1;
    }
  }

  unsigned *buf = pState->objcodeBuf;
  int numBytes = pState->objcodeLen;
  int const SZ = sizeof(unsigned);
  char *recBuf = pState->textRecBuf + curLen*2;

  int i;
  for(i=0; i<numBytes/SZ; ++i, recBuf+=SZ*2) {
    sprintf(recBuf, "%0*X", SZ*2, buf[i]);
  }
  if(numBytes%SZ) {
    int r = numBytes%SZ;
    unsigned mask = ~(~0u<<(r*8));
    sprintf(recBuf, "%0*X", r*2, buf[i]&mask);
  }

  pState->curTextRecordLen = curLen + numBytes;
  pState->objcodeLen = 0;

  return 0;
}

static void printModRec(FILE *objOut, AssembleState *pState) {
  MRec **recList = (MRec **)pState->mRecVec->arr;
  int size = pState->mRecVec->size;
  int i;
  for(i=0; i<size; ++i) {
    fprintf(objOut, "M%06X%02X\n", recList[i]->start, recList[i]->halfBytes);
  }
}

static void printEndRec(FILE *objOut, AssembleState *pState) {
  fprintf(objOut, "E%06X\n", pState->pcAddr);
}