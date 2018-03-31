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
  while((ch=path[i]) != '\0') 
  for(i=0; (ch=path[i]) != '\0'; ++i) {
    if(ch == '.') j=i;
  }

  if(j == -1 || strcmp(DEFAULT_ASM_EXT, path+j+1) != 0) {
    printErrMsg(EXT_MISMATCH);
    return INTERNAL_COMMAND_ERROR;
  }

  FILE *asmIn, *lstOut, *objOut;
  char *lstPath = NULL, *objPath = NULL;

  asmIn = fopen(args.argv[1], "r");
  if(asmIn == NULL) {
    printErrMsg(FILE_OPEN_ERR);
    return INTERNAL_COMMAND_ERROR;
  }

  lstPath = malloc(sizeof(DEFAULT_LST_EXT)+1);
  objPath = malloc(sizeof(DEFAULT_OBJ_EXT)+1);
  if(lstPath == NULL || objPath == NULL) {
    printErrMsg(ALLOC_ERR);
    return INTERNAL_COMMAND_ERROR;
  }

  sprintf(lstPath, "%*s.%s", j, path, DEFAULT_LST_EXT);
  lstOut = fopen(lstPath, "w");

  sprintf(objPath, "%*s.%s", j, path, DEFAULT_OBJ_EXT);
  objOut = fopen(objPath, "w");

  if(lstOut == NULL || objOut == NULL) {
    printErrMsg(FILE_OPEN_ERR);
    free(lstPath);
    free(objPath);
    return INTERNAL_COMMAND_ERROR;
  }

  unsigned asmErr = assemble(pContext, asmIn, lstOut, objOut);

  fclose(asmIn);
  fclose(lstOut);
  fclose(objOut);
  if(asmErr != 0) {
    remove(lstPath);
    remove(objPath);
  }
  free(lstPath);
  free(objPath);

  return asmErr ? INTERNAL_COMMAND_ERROR : 0;
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
