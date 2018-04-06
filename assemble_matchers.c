#include "./assemble.h"

#include <ctype.h>
#include <stdbool.h>

bool identifierMatcher(char *str, int n, int *i_r) {
  if(!isalpha(str[0])) {
    *i_r = 0;
    return false;
  }
  int i = 1;
  while((n<=0 || i<n) && str[i]) {
    if(!isalnum(str[i])) {
      *i_r = i;
      return false;
    }
    ++i;
  }

  *i_r = i;
  return true;
}

bool charLiteralMatcher(char *str, int n, int *i_r) {
  int i = 0;
  if(str[i] != 'C' || str[++i] != '\'') {
    *i_r = i;
    return false;
  }

  ++i;
  while((n<=0 || i<n) && str[i]) {
    if(str[i] == '\'') {
      *i_r = ++i;
      return !((n<=0 || i<n) && str[i]);
    }
    ++i;
  }

  *i_r = i;
  return false;
}

int hexLiteralMatcher(char *str, int n, int *i_r) {
  int i = 0;
  if(str[i] != 'X' || str[++i] != '\'') {
    *i_r = i;
    return false;
  }

  ++i;
  while((n<=0 || i<n) && str[i]) {
    if(str[i] == '\'') {
      *i_r = ++i;
      return !((n<=0 || i<n) && str[i]);
    }
    if(!isxdigit(str[i])) break;
    ++i;
  }

  *i_r = i;
  return false;
}

int integerMatcher(char *str, int n, int *i_r) {
  int i = 0;
  while((n<=0 || i<n) && str[i]) {
    if(!isdigit(str[i])) {
      *i_r = i;
      return false;
    }
    ++i;
  }

  *i_r = i;
  return true;
}