#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./commands.h"
#include "./shell-context.h"
#include "./assemble.h"

typedef enum _ERR_FLAG {
  EXT_MISMATCH = 1<<0,
  FILE_OPEN_ERR = 1<<1,
  ALLOC_ERR = 1<<2
} ERR_FLAG;

static void printErrMsg(ERR_FLAG flag);

EXIT_FLAG COMMAND_ASSEMBLE(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 2) {
    return UNKNOWN_ARGUMENT;
  }

  int i=0, j=-1;
  char ch;
  char const* path = args.argv[1];
  ERR_FLAG errFlag = 0;
  unsigned asmErr = 0;

  FILE *asmIn = NULL, *lstOut = NULL, *objOut = NULL;
  char *lstPath = NULL, *objPath = NULL;

  // 파일명 문자열의 확장자 부분들 찾는다
  for(i=0; (ch=path[i]) != '\0'; ++i) {
    if(ch == '.') j=i;
  }

  if(j == -1 || strcmp(DEFAULT_ASM_EXT, path+j+1) != 0) {
    errFlag = EXT_MISMATCH;
    goto cleanup;
  }

  asmIn = fopen(args.argv[1], "r");
  if(asmIn == NULL) {
    errFlag = FILE_OPEN_ERR;
    goto cleanup;
  }

  lstPath = malloc(j+1+sizeof(DEFAULT_LST_EXT));
  objPath = malloc(j+1+sizeof(DEFAULT_OBJ_EXT));
  if(lstPath == NULL || objPath == NULL) {
    errFlag = ALLOC_ERR;
    goto cleanup;
  }

  sprintf(lstPath, "%.*s.%s", j, path, DEFAULT_LST_EXT);
  lstOut = fopen(lstPath, "w");

  sprintf(objPath, "%.*s.%s", j, path, DEFAULT_OBJ_EXT);
  objOut = fopen(objPath, "w");

  if(lstOut == NULL || objOut == NULL) {
    errFlag = FILE_OPEN_ERR;
    goto cleanup;
  }

  // 실제 assemble의 시작
  asmErr = assemble(pContext, asmIn, lstOut, objOut);

cleanup:
  if(asmIn != NULL) fclose(asmIn);
  if(lstOut != NULL) fclose(lstOut);
  if(objOut != NULL) fclose(objOut);
  if(asmErr != 0) {
    remove(lstPath);
    remove(objPath);
  }
  free(lstPath);
  free(objPath);

  if(errFlag != 0) {
    printErrMsg(errFlag);
  }

  return errFlag || asmErr ? INTERNAL_COMMAND_ERROR : 0;
}

static void printErrMsg(ERR_FLAG flag) {
  if(flag == 0) return;
  if(flag & EXT_MISMATCH) {
    printf("-assemble: only extension .%s is permitted\n", DEFAULT_ASM_EXT);
  }
  else if(flag & FILE_OPEN_ERR) {
    printf("-assemble: error opening file\n");
  }
  else if(flag & ALLOC_ERR) {
    printf("-assemble: allocation fail\n");
  }
}
