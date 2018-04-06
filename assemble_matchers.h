#ifndef _ASSEMBLE_MATCHERS_
#define _ASSEMBLE_MATCHERS_

#include <stdbool.h>

typedef bool (*MATCHER_FUNC)(char const *str, int n, int *i_r);

bool identifierMatcher(char const *str, int *i_r);
bool charLiteralMatcher(char const *str, int *i_r);
bool hexLiteralMatcher(char const *str, int *i_r);
bool integerMatcher(char const *str, int *i_r);

bool operandMatcher_F1_DEFAULT(char const *str, int *i_r);
bool operandMatcher_F2_DEFAULT(char const *str, int *i_r);
bool operandMatcher_F34_DEFAULT(char const *str, int *i_r);
bool operandMatcher_F2_SHIFT(char const *str, int *i_r);
bool operandMatcher_F1_SINGLE_REG(char const *str, int *i_r);
bool operandMatcher_F1_SINGLE_CONST(char const *str, int *i_r);

#endif