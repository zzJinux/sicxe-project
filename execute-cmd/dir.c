/** dir.c
 *  dir.h 구현
 */
#include "dir.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dirent.h"
#include "sys/stat.h"

#define MAX_WIDTH (128)

void printDir(char const *path) {
  // 디렉토리 스트림을 open
  DIR *dp = opendir(path);

  // 디렉토리를 찾을 수 없음
  if(dp == NULL) {
    printf("dir: cannot access '%s'\n", path);
    return;
  }

  int maxLen = 0; // column-major order로 출력할 때 column의 길이에 계산됨
  struct dirent *ep; // 디렉토리 항목정보를 가리킴

  // dp 스트림에 대해 NULL이 나올 때까지 항목을 읽어옴
  while((ep = readdir(dp))) {
    int len;
    struct stat st;
    len = strlen(ep->d_name);

    stat(ep->d_name, &st); // 디렉토리 항목 정보를 불러온다

    // 디렉토리나 실행파일들은 출력 시 끝에 장식문자가 붙으므로 1 증가시켜줌
    if(st.st_mode & S_IFDIR || (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
      ++len;
    }
    // maxLen 계산
    if(maxLen < len) maxLen = len;
  }

  // 디렉토리 항목 출력 시 행, 열의 개수
  int ROW, COL;
  // 열 개수는 항목의 길이 + 공백(2)로 계산된다.
  COL = MAX_WIDTH / (maxLen+2);
  // 0으로 계산되면 강제로 1로 바꿔줌
  if(COL == 0) COL = 1;
  
  // scandir를 통해 사전순으로 정렬된 항목들의 리스트를 받아올 수 있음
  struct dirent **entryList;
  int num = scandir(path, &entryList, NULL, alphasort);
  ROW = (num + COL-1)/COL; // 행 개수 계산

  int r, c, i; // 행, 열, 항목 인덱스
  for(r=0; r<ROW; ++r) {
    for(c=0; c<COL; ++c) {
      i = r + (ROW*c);
      if(i >= num) continue;

      /* st_mode 정보를 기반으로 끝에 들어갈 문자를 정한다 */
      char ch = ' ';
      struct stat st;
      stat(entryList[i]->d_name, &st);
      if(st.st_mode & S_IFDIR) ch = '/'; // 디렉토리
      else if(st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) ch = '*'; // 실행파일

      // prt에 출력된 문자의 개수를 저장
      int prt = printf("%s%c", entryList[i]->d_name, ch);
      // 열 길이(maxLen+2) 단위로 정렬되도록 공백 출력
      printf("%*s", maxLen+2-prt, "");
    }
    putchar('\n');
  }

  // scandir는 내부적으로 동적할당을 하므로 정리해준다
  for(i=0; i<num; ++i) free(entryList[i]);
  free(entryList);

  // 디렉토리 스트림을 close
  closedir(dp);
}