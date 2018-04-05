#include "./symtab.h"

#include <string.h>
#include <stdlib.h>
#include "./util.h"

#define NUM_BUCKETS 20
HashTable *initSymbolTable() {
  return initHashTable(NUM_BUCKETS);
}

SymbolDef *findSymbol(HashTable *symtab, char const *sym) {
  int h = hash_adler32(sym, symtab->size);
  LLNodePtr p = symtab->buckets[h];
  
  while(p != NULL) {
    if(strcmp(((SymbolDef *)p->key)->name, sym) == 0) {
      return p->key;
    }
    p = p->next;
  }

  return NULL;
}

LLNodePtr addSymbol(HashTable *symtab, char const *sym, int loc) {
  LLNodePtr nn = malloc(sizeof(LLNode));
  SymbolDef *key = malloc(sizeof(SymbolDef));
  char *buf = malloc(strlen(sym)+1);
  if(nn == NULL || key == NULL) return NULL;

  nn->key = key;
  nn->next = NULL;
  strcpy(buf, sym);
  key->name = buf;
  key->loc = loc;

  int h = hash_adler32(buf, symtab->size);
  LLNodePtr *dp =  symtab->buckets + h, p;

  p = *dp;
  if(p == NULL) {
    *dp = nn;
  }
  else {
    while(p->next != NULL) p = p->next;
    p->next = nn;
  }

  return nn;
}

static int SymbolDefCmp(const void *a, const void *b) {
  return strcmp((*(SymbolDef **)b)->name, (*(SymbolDef **)a)->name);
}

int printSymbolTable(HashTable *symtab) {
  int sz = symtab->size;
  int i, cnt;
  for(i=0, cnt=0; i<sz; ++i) {
    LLNodePtr p = symtab->buckets[i];
    while(p != NULL) {
      ++cnt;
      p = p->next;
    }
  }

  SymbolDef **sortArr = malloc(cnt * sizeof(SymbolDef *));
  if(sortArr == NULL) {
    return -1;
  }

  for(i=0, cnt=0; i<sz; ++i) {
    LLNodePtr p = symtab->buckets[i];
    while(p != NULL) {
      sortArr[cnt++] = p->key;
      p = p->next;
    }
  }

  qsort(sortArr, cnt, sizeof(SymbolDef *), SymbolDefCmp);
  for(i=0; i<cnt; ++i) {
    printf("\t%s\t%04X\n", sortArr[i]->name, sortArr[i]->loc);
  }

  free(sortArr);
  return 0;
}

static void cleanupSymbol(SymbolDef *sym) {
  free((void *)sym->name);
  free(sym);
}

void emptySymbolTable(HashTable *symtab) {
  emptyHashTable(symtab, (CLEANUP_FUNC)cleanupSymbol);
}

void cleanupSymbolTable(HashTable *symtab) {
  cleanupHashTable(symtab, (CLEANUP_FUNC)cleanupSymbol);
}