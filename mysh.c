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
char buf[BUF_SIZE];
int TOKEN_SIZE;

//
// devide each token with space from input stream
//
void devide_tokens(char *buf, char *tokens[]) {
  char *tmp = NULL;
  TOKEN_SIZE = 0;

  tmp = strtok(buf, " ");
  tokens[TOKEN_SIZE] = tmp;
  TOKEN_SIZE++;
  while(tmp != NULL){
    tmp = strtok(NULL, " ");
    if (tmp != NULL) {
      tokens[TOKEN_SIZE] = tmp;
      TOKEN_SIZE++;
    }
  }

  tokens[TOKEN_SIZE] = NULL;
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

char *open_file(char *file) {
  int size, f_fd;
  char *text;
  struct stat info;

  f_fd = open(file, O_RDONLY);
  fstat(f_fd, &info);
  if (f_fd >= 0 && (info.st_mode & S_IFREG)) {
    read(f_fd, text, sizeof(text));
  } else {
    printf("Error occured. check: %s\n", file);
  }
  close(f_fd);

  return text;
}

void write_file(char *file) {
  char str[256];
  int size, c_fd;
  struct stat info;

  c_fd = open(file, O_WRONLY);
  if (c_fd < 0) {
    creat(file, 0644);
    c_fd = open(file, O_WRONLY);
  }
  fstat(c_fd, &info);
  if (c_fd >= 0 && (info.st_mode & S_IFREG)) {
    write(c_fd, str, size);
  } else {
    printf("Error occured. check: %s\n", file);
  }
  close(c_fd);
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

      for (i = 0; i < TOKEN_SIZE; ++i){
        if (strncmp(tokens[i], "<\0", 2) == 0) {
          // strcpy(tokens[i], open_file(tokens[i + 1]));
          // tokens[i] = open_file(tokens[i + 1]);
          printf("%s\n", open_file(tokens[i + 1]));
        }
        if (strncmp(tokens[i], ">\0", 2) == 0) {
          write_file(tokens[i + 1]);
        }
      }

      for (i = 0; i < TOKEN_SIZE; ++i){
        printf("%s\n", tokens[i]);
      }

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
