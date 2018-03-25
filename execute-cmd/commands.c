#include "commands.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dir.h"
#include "shell-context/history.h"
#include "shell-context/memory.h"
#include "shell-context/opcode.h"

static ShellCmd const SHELL_COMMANDS[] = {
  {"help", "h", "h[elp]", COMMAND_HELP},
  {"dir", "d", "d[ir]", COMMAND_DIR},
  {"quit", "q", "q[uit]", COMMAND_QUIT},
  {"history", "hi", "hi[story]", COMMAND_HISTORY},
  {"dump", "du", "du[mp] [start, end]", COMMAND_DUMP},
  {"edit", "e", "e[dit] address, value", COMMAND_EDIT},
  {"fill", "f", "f[ill] start, end, value", COMMAND_FILL},
  {"reset", NULL, "reset", COMMAND_RESET},
  {"opcode", NULL, "opcode mnemoic", COMMAND_OPCODE},
  {"opcodelist", NULL, "opcodelist", COMMAND_OPCODELIST}
};

int getShellCmdCount() {
  return sizeof(SHELL_COMMANDS)/sizeof(ShellCmd);
}

ShellCmd const *getShellCmdList() {
  return SHELL_COMMANDS;
}

static EXIT_FLAG parseHex_u20(char const *hex, unsigned *p) {
  unsigned x = 0;
  char c;
  int i;
  for(i=0; isxdigit(c=hex[i]) && i<5; ++i) {
    x = x*16 + c - (c < 'A' ? '0' : (c < 'a' ? 'A' : 'a') - 10); 
  }
  if(c != '\0') {
    if(i>=5) return ARGUMENT_TOO_LONG;
    else return NOT_HEX;
  }
  *p = x;
  return 0;
}

static EXIT_FLAG parseAddrPoint(OFFSET *p, char const* str, OFFSET BOUNDARY) {
  EXIT_FLAG f = parseHex_u20(str, p);
  if(f & INVALID_ARGUMENT) {
    return f;
  }
  if(*p >= BOUNDARY) {
    return OUT_OF_RANGE;
  }
  return 0;
}

static EXIT_FLAG parseAddrRange(OFFSET *ps, OFFSET *pe, char const *s_start, char const *s_end, OFFSET BOUNDARY) {
  EXIT_FLAG f;
  f = parseAddrPoint(pe, s_end, BOUNDARY);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  f = parseAddrPoint(ps, s_start, *pe+1);
  if(f & INVALID_ARGUMENT) {
    return f;
  }
  return 0;
}

static EXIT_FLAG parseByte(BYTE *p, char const *str) {
  EXIT_FLAG f;
  unsigned x;
  f = parseHex_u20(str, &x);
  if(f & INVALID_ARGUMENT) {
    return f;
  }
  if(x > 0xFF) {
    return OUT_OF_RANGE;
  }
  *p = (BYTE)x;
  return 0;
}

EXIT_FLAG COMMAND_HELP(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  int i, cnt = getShellCmdCount();
  for(i=0; i<cnt; ++i) {
    printf("%s\n", SHELL_COMMANDS[i].desc);
  }

  return 0;
}

EXIT_FLAG COMMAND_DIR(ShellContextPtr pContext, const Arguments args) {
  printDir(".");
  return 0;
}

EXIT_FLAG COMMAND_QUIT(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }
  updateHistory(pContext, args.RAW, args.RAW_LEN);
  return QUIT_SHELL;
}

EXIT_FLAG COMMAND_HISTORY(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }
  updateHistory(pContext, args.RAW, args.RAW_LEN);
  printHistory(pContext);
  return 0;
}

EXIT_FLAG COMMAND_DUMP(ShellContextPtr pContext, const Arguments args) {
  if(args.argc > 3) {
    return UNKNOWN_ARGUMENT;
  }

  if(args.argc == 1) {
    dumpMemory(pContext, 0);
  }
  else if(args.argc == 2) {
    OFFSET start;
    EXIT_FLAG f = parseAddrPoint(&start, args.argv[1], pContext->memSize);
    if(f & INVALID_ARGUMENT) {
      return f;
    }

    dumpMemory(pContext, 1, start);
  }
  else {
    OFFSET start, end;
    EXIT_FLAG f = parseAddrRange(&start, &end, args.argv[1], args.argv[2], pContext->memSize);
    if(f & INVALID_ARGUMENT) {
      return f;
    }

    dumpMemory(pContext, 2, start, end);
  }
  return 0;
}

EXIT_FLAG COMMAND_EDIT(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 3) {
    return UNKNOWN_ARGUMENT;
  }

  EXIT_FLAG f;
  OFFSET pos;
  BYTE value;
  f = parseAddrPoint(&pos, args.argv[1], pContext->memSize);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  f = parseByte(&value, args.argv[2]);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  editMemory(pContext, pos, value);
  return 0;
}
EXIT_FLAG COMMAND_FILL(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 4) {
    return UNKNOWN_ARGUMENT;
  }

  EXIT_FLAG f;
  OFFSET start, end;
  BYTE value;
  f = parseAddrRange(&start, &end, args.argv[1], args.argv[2], pContext->memSize);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  f = parseByte(&value, args.argv[3]);
  if(f & INVALID_ARGUMENT) {
    return f;
  }

  fillMemory(pContext, start, end, value);
  return 0;
}

EXIT_FLAG COMMAND_RESET(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  resetMemory(pContext);
  return 0;
}

EXIT_FLAG COMMAND_OPCODE(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 2) {
    return UNKNOWN_ARGUMENT;
  }

  char *str = args.argv[1];
  while(isupper(*str)) ++str;
  if(*str != '\0') {
    printf("-note: mnemonic should be lowercase alphabet\n");
    return UNKNOWN_ARGUMENT;
  }

  int opcode = findOpcode(pContext, args.argv[1]);
  if(opcode != -1) {
    printf("opcode is %02X\n", opcode);
  }
  else {
    printf("opcode is not defined for %s\n", str);
  }
  return 0;
} 
EXIT_FLAG COMMAND_OPCODELIST(ShellContextPtr pContext, const Arguments args) {
  if(args.argc != 1) {
    return UNKNOWN_ARGUMENT;
  }

  printOpcodeList(pContext);
  return 0;
}