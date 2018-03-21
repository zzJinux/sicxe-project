#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

typedef struct _Arguments {
  char const *RAW; int RAW_LEN;
  int argc; char **argv;
} Arguments;

typedef enum _EXIT_FLAG {
  EXECUTE_ERROR = 1,
  UNKNOWN_COMMAND = 3,
  UNKNOWN_ARGUMENT = 5,
  QUIT_SHELL = 16
} EXIT_FLAG;

typedef struct _HistNode {
  char *text;
  struct _HistNode *next;
} HistNode, *HistNodePtr;

typedef struct _ShellContext {
  HistNodePtr histRootNode;
  HistNodePtr histCurrentNode;
} ShellContext, *ShellContextPtr;

typedef EXIT_FLAG (*COMMAND_FUNC)(ShellContextPtr, const Arguments);

typedef struct _ShellCmd {
  char const *fullname, *shorthand, *desc;
  COMMAND_FUNC commandFunc;
} ShellCmd;


#endif