#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "./util.h"

typedef struct _SymbolDef {
  char const *name;
  int loc;
} SymbolDef;

HashTable *initSymbolTable(void);

SymbolDef *findSymbol(HashTable *symtab, char const *sym);

LLNodePtr addSymbol(HashTable *symtab, char const *sym, OFFSET loc);

void printSymbolTable(HashTable *symtab);

void cleanupSymbolTable(HashTable *symtab);

#endif