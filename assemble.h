#ifndef _ASSEMBLE_H_
#define _ASSEMBLE_H_
#include <stdio.h>
#include "./shell-context.h"
#include "./typedefs.h"

#define DEFAULT_ASM_EXT "asm"
#define DEFAULT_LST_EXT "lst"
#define DEFAULT_OBJ_EXT "obj"

ERROR_CODE assemble(ShellContextPtr pContext, FILE *asmIn, FILE *lstOut, FILE *objOut);

#endif
