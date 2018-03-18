#include <stdio.h>
#include <string.h>
#define MAX_LEN 64
#define EXIT_FLAG 1

// tokenize: 명령어 텍스트를 토큰들의 배열로 파싱
void tokenize(char cmd[], int *pArgc, char *argv[]);
// execute: 토큰화된 명령어를 인자로 받아 실제 명령 수행
unsigned char execute(int argc, char *argv[]);
// cleanup: 동적할당된 자원 해제 (tokenize에서 수행된 동적할당)
void cleanup(int argc, char *argv[]);

int main() {
  printf("sicsim> ");
  while(1) {
    // stdin으로부터 최대 MAX_LEN 길이의 커맨드 입력받음
    char cmd[MAX_LEN+1];
    fgets(cmd, MAX_LEN+1, stdin); // cmd는 null-termination이 보장됨
    int len = strlen(cmd);

    int argc;
    char **argv;
    tokenize(cmd, &argc, argv);

    unsigned char flag = execute(argc, argv);
    if(flag & EXIT_FLAG) {
      break;
    }

    cleanup(argc, argv);
  }
  return 0;
}

void tokenize(char cmd[], int *pArgc, char *argv[]) {
}
