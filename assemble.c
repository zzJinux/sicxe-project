#include "./assemble.h"

#include <stdio.h>
#include "./typedefs.h"
#include "./shell-context.h"
#include "./opcode.h"
#include "./symtab.h"

#include "./assemble_types.h"
#include "./assemble_consts.h"
#include "./assemble_errs.h"

char const *SYNTAX_ERROR_MESSAGES[] = {
  "<no-error>",
  "START directive at wrong place",
  "PROGRAM_NAME too long",
  "LOC is out of range",
  "invalid operand format",
  "invalid symbol name",
  "operand is missing",
  "symbol is already defined",
  "opcode undefined",
  "symbol undefined",
  "constant is out-of-range",
  "register undefined"
};


ERROR_CODE assemble(ShellContextPtr pContext, FILE *asmIn, FILE *lstOut, FILE *objOut) {
  ASSEMBLE_ERROR errCode;

  // Statement를 저장할 동적배열을 생성함
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
  _assemble_printErrMsg(errCode, "");
  cleanupVec(stVec);
  return errCode;

  return 0;
}

void _assemble_printErrMsg(ASSEMBLE_ERROR code, char const* detail) {
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

void _assemble_printSyntaxErrMsg(SYNTAX_ERROR code, Statement *st, int colNo) {
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
