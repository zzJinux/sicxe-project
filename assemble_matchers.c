#include "./assemble_matchers.h"

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "./util.h"

static bool isEOS(char const *str, int i) {
  return !str[i] || isspace(str[i]);
}

bool identifierMatcher(char const *str, int *i_r) {
  int _;
  if(i_r == NULL) { i_r = &_; }

  if(!isalpha(str[0])) {
    *i_r = 0;
    return false;
  }
  int i = 1;
  while(!isEOS(str, i)) {
    if(!isalnum(str[i])) {
      *i_r = i;
      return false;
    }
    ++i;
  }

  *i_r = i;
  return true;
}

bool charLiteralMatcher(char const *str, int *i_r) {
  int _;
  if(i_r == NULL) { i_r = &_; }

  int i = 0;
  if(str[i] != 'C' || str[++i] != '\'') {
    *i_r = i;
    return false;
  }

  ++i;
  while(!isEOS(str, i)) {
    if(str[i] == '\'') {
      *i_r = ++i;
      return isEOS(str, i) && i>3;
    }
    else if(isspace(str[i+1])) {
      ++i;
      jumpBlank(str, &i);
      continue;
    }
    ++i;
  }

  *i_r = i;
  return false;
}

bool hexLiteralMatcher(char const *str, int *i_r) {
  int _;
  if(i_r == NULL) { i_r = &_; }

  int i = 0;
  if(str[i] != 'X' || str[++i] != '\'') {
    *i_r = i;
    return false;
  }

  ++i;
  while(!isEOS(str, i)) {
    if(str[i] == '\'') {
      *i_r = ++i;
      return isEOS(str, i) && i>3;
    }
    if(!isxdigit(str[i])) break;
    ++i;
  }

  *i_r = i;
  return false;
}

bool integerMatcher(char const *str, int *i_r) {
  int _;
  if(i_r == NULL) { i_r = &_; }

  int i = 0;
  char const *s = str;
  if(str[0] == '-' || str[0] == '+') ++s;

  while(!isEOS(s, i)) {
    if(!isdigit(s[i])) {
      *i_r = i + (s-str);
      return false;
    }
    ++i;
  }

  if(i == 0) {
    *i_r = i + (s-str);
    return false;
  }

  *i_r = i + (s-str);
  return true;
}

bool operandMatcher(char const *str, int *i_r) {
  int _;
  if(i_r == NULL) { i_r = &_; }

  int i = 0, len = 0;

  if(charLiteralMatcher(str+i, &len)) {
    i += len;
    if(jumpBlank(str+i, &len)) {
      *i_r = i + len;
      return false;
    }
    else {
      *i_r = i;
      return true;
    }
  }

  if(hexLiteralMatcher(str+i, &len)) {
    i += len;
    if(jumpBlank(str+i, &len)) {
      *i_r = i + len;
      return false;
    }
    else {
      *i_r = i;
      return true;
    }
  }

  if(str[i] == '#' || str[i] == '@') {
    ++i;
  }

  char ch;
  if(
    !identifierMatcher(str+i, &len) &&
    !integerMatcher(str+i, &len)
  ) {
    *i_r = i + len;
    return false;
  }
  i += len;

  len = 0;
  ch = jumpBlank(str+i, &len);
  if(!ch) {
    *i_r = i;
    return true;
  }
  i += len;

  if(str[i] != ',') {
    *i_r = i;
    return false;
  }

  len = 0;
  ch = jumpBlank(str+i, &len);
  i += len;

  if(
    !identifierMatcher(str+i, &len) &&
    !integerMatcher(str+i, &len)
  ) {
    *i_r = len + i;
    return false;
  }
  i += len;

  len = 0;
  ch = jumpBlank(str+i, &len);
  if(!ch) {
    *i_r = i;
    return true;
  }
  i += len;

  *i_r = i;
  return false;
}