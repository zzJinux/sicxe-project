#include "dir.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dirent.h"
#include "sys/stat.h"

#define MAX_WIDTH (128)

void printDir(char const *path) {
  DIR *dp = opendir(path);
  if(dp == NULL) {
    printf("dir: cannot access '%s'\n", path);
    return;
  }

  int maxLen = 0;
  struct dirent *ep;
  while((ep = readdir(dp))) {
    int len;
    struct stat st;
    len = strlen(ep->d_name);

    stat(ep->d_name, &st);
    if(st.st_mode & S_IFDIR || (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
      ++len;
    }
    if(maxLen < len) maxLen = len;
  }

  int ROW, COL;
  COL = MAX_WIDTH / (maxLen+2);
  if(COL == 0) COL = 1;
  
  struct dirent **entryList;
  int num = scandir(path, &entryList, NULL, alphasort);
  ROW = (num + COL-1)/COL;

  int r, c, i;
  for(r=0; r<ROW; ++r) {
    for(c=0; c<COL; ++c) {
      i = r + (ROW*c);
      if(i >= num) continue;

      char ch = ' ';
      struct stat st;
      stat(entryList[i]->d_name, &st);
      if(st.st_mode & S_IFDIR) ch = '/';
      else if(st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) ch = '*';

      int prt = printf("%s%c", entryList[i]->d_name, ch);
      printf("%*s", maxLen+2-prt, "");
    }
    putchar('\n');
  }

  for(i=0; i<num; ++i) free(entryList[i]);
  free(entryList);

  closedir(dp);
}