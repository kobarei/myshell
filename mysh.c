#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUF_SIZE 256

bool prom = true;

bool is_exist(char *path_cmd);
char *read_path(char *cmd);

int main(int argc, char const *argv[]) {
  char cmd[BUF_SIZE];
  int pid;
  int stats;

  while(prom){
    write(1, "mysh> ", 6);
    read(0, cmd, BUF_SIZE);
    if (strcmp(cmd, "exit\n") == 0)
      break;

    char *path = read_path(cmd);

    pid = fork();
    if (pid == -1) {
      write(2, "failed to fork\n", 15);
    } else if (pid == 0) {
      execl("/bin/ls", "ls", NULL);
    }
    waitpid(pid, &stats, 0);
  }

  return 0;
}

//
// is_exist command
//
bool is_exist(char *path_cmd) {
  bool aru;
  int fd;
  struct stat info;

  printf("path_cmd = %s", path_cmd);
  printf("%lu\n", strlen(path_cmd));
  fd = open(path_cmd, O_RDONLY);
  fstat(fd, &info);
  if (info.st_mode & S_IFREG) {
    aru = true;
    printf("atta\n");
  } else {
    aru = false;
    printf("nai\n");
  }
  close(fd);

  return aru;
}

//
// path + cmd
//
char *read_path(char *cmd) {
  static char str[BUF_SIZE];
  // char *path_cmd;
  char path_cmd[BUF_SIZE];
  int f_fd;
  int i;
  bool aru;

  f_fd = open(".path", O_RDONLY);
  read(f_fd, str, sizeof(str));
  close(f_fd);

  for (i = 0; i < strlen(str); ++i){
    if (strncmp(&str[i], "\n", 1) == 0) {
      strcat(path_cmd, cmd);
      strcat(path_cmd, "\0");
      is_exist(path_cmd);
      strcpy(path_cmd, "");
    } else {
      strncat(path_cmd, &str[i], 1);
    }
  }

  // aru = is_exist(path_cmd);

  return str;
}
