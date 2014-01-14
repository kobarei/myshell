#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#define BUF_SIZE 256

char *PATH;
struct stat info;
char buf[BUF_SIZE];

void devide_tokens(char *buf, char *tokens[]) {
  int i = 0;
  char *tmp = NULL;

  tmp = strtok(buf, " ");
  tokens[i] = tmp;
  i++;
  while(tmp != NULL){
    tmp = strtok(NULL, " ");
    if (tmp != NULL) {
      tokens[i] = tmp;
      i++;
    }
  }

  tokens[i] = NULL;
}

//
// read path file
//
void read_path() {
  int f_fd;
  char path_set[BUF_SIZE];

  f_fd = open(".path", O_RDONLY);
  if (f_fd == -1) {
    printf(".path Not Found\n");
  } else {
    read(f_fd, path_set, sizeof(path_set));
    close(f_fd);
    PATH = path_set;
    setenv("PATH", PATH, 0);
  }
}

int main(int argc, char const *argv[]) {
  int i;
  int pid;
  int stats;
  char *tokens[BUF_SIZE];

  read_path();

  while(1){
    write(1, "mysh> ", sizeof("mysh> "));
    read(0, buf, BUF_SIZE);

    // replace \n with \0
    for (i = 0; i < strlen(buf); ++i){
      if (strncmp(&buf[i], "\n", 1) == 0) {
        strncpy(&buf[i], "\0", 1);
        break;
      }
    }

    if (strncmp(buf, "\0", 1) != 0) {
      if (strcmp(buf, "exit") == 0) {
        break;
      }

      devide_tokens(buf, tokens);

      pid = fork();
      if (pid == -1) {
        write(2, "failed to fork\n", 15);
      } else if (pid == 0) {
        execvp(tokens[0], tokens);
        printf("%s: %s\n", tokens[0], strerror(errno));
      }
      waitpid(pid, &stats, 0);
    }

  }

  return 0;
}
