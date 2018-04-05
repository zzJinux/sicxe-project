#include "./assemble.h"
#include "./assemble_types.h"
#include "./assemble_consts.h"
#include "./assemble_errs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./typedefs.h"
#include "./shell-context.h"
#include "./opcode.h"
#include "./symtab.h"
#include "./util.h"

static void printErrMsg(ASSEMBLE_ERROR code, char const* detail);
static void printSyntaxErrMsg(SYNTAX_ERROR code, Statement *st, int colNo);

#define _ASSEMBLE_SUB_IMPL_
  #include "./assemble_pass1.c"
  #include "./assemble_pass2.c"
#undef _ASSEMBLE_SUB_IMPL_

ERROR_CODE assemble(ShellContextPtr pContext, FILE *asmIn, FILE *lstOut, FILE *objOut) {
  ASSEMBLE_ERROR errCode;

  Vec *stVec = initVec(32, (CLEANUP_FUNC)cleanupStatement);
  if(stVec == NULL) {
    errCode = ALLOC_FAIL;
    goto cleanup;
  }

  errCode = assemble_pass1(asmIn, pContext->opcodeTable, pContext->symbolTable, stVec);
  if(errCode) {
    goto cleanup;
  }
  errCode = assemble_pass2(lstOut, objOut, pContext->opcodeTable, pContext->symbolTable, stVec);
  if(errCode) {
    goto cleanup;
  }

cleanup:
  printErrMsg(errCode, "");
  cleanupVec(stVec);
  return errCode;

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
  if(colNo > 0) printf("%*c\n", prefix+colNo, '^');
}
