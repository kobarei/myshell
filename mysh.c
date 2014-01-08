#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUF_SIZE 256

int prom = 1;

void left_prompt();
void is_exist();
void j_exit();
char *read_path();

int main(int argc, char const *argv[]) {
  char str[BUF_SIZE];
  int i;
  int pid;
  int stats;

  char *path = read_path();

  for (i = 0; i < strlen(path); ++i){
    printf("%c", path[i]);
    if (strncmp(&path[i], "\n", 1) == 0)
      printf("改行");
  }

  while(prom){
    write(1, "mysh> ", 6);
    read(0, str, BUF_SIZE);
    if (strcmp(str, "exit\n") == 0)
      break;

    pid = fork();
    if (pid == -1) {
      write(2, "failed to fork\n", 15);
    } else if (pid == 0) {
      execl("/bin/ls", "ls");
    }
    waitpid(pid, &stats, 0);
  }
  return 0;
}

//
// output left prompt
//
void left_prompt() {
}

//
// is_exist command
//
void is_exist(char* str) {
  int fd;

}

char *read_path() {
  static char str[256];
  int f_fd;

  f_fd = open(".path", O_RDONLY);
  read(f_fd, str, sizeof(str));
  close(f_fd);
  strcat(str, "\n");

  return str;
}

//
// EXIT if 'exit' input
//
void j_exit(char* str) {
  if (strcmp(str, "exit\n") == 0)
    prom = 0;
}
