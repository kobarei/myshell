#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define BUF_SIZE 256

bool prom = true;
struct stat info;
bool cmd_aru = false;

bool is_exist(char *path_cmd);
char *build_path(char *cmd);

int main(int argc, char const *argv[]) {
  static char cmd[BUF_SIZE];
  char path_cmd[BUF_SIZE];
  char *args[BUF_SIZE];
  int pid;
  int stats;

  while(prom){
    strcpy(path_cmd, "");
    write(1, "mysh> ", 6);
    read(0, cmd, BUF_SIZE);
    if (strncmp(cmd, "\n", 1) != 0) {
      strcpy(&cmd[strlen(cmd) - 1], "\0");
      if (strcmp(cmd, "exit") == 0)
        break;

      strcpy(path_cmd, build_path(cmd));

      if (cmd_aru) {
        args[0] = cmd;
        args[1] = NULL;
        pid = fork();
        if (pid == -1) {
          write(2, "failed to fork\n", 15);
        } else if (pid == 0) {
          execv(path_cmd, args);
        }
        waitpid(pid, &stats, 0);
      } else {
        printf("%s: Command Not Found\n", cmd);
      }
    }

  }

  return 0;
}

//
// is_exist command
//
bool is_exist(char *path_cmd) {
  int fd;

  cmd_aru = false;
  fd = open(path_cmd, O_RDONLY);
  if (fd >= 0) {
    cmd_aru = true;
  }
  close(fd);

  return cmd_aru;
}

//
// path + cmd
//
char *build_path(char *cmd) {
  static char str[BUF_SIZE];
  static char path_cmd[BUF_SIZE];
  int f_fd;
  int i;

  f_fd = open(".path", O_RDONLY);
  read(f_fd, str, sizeof(str));
  close(f_fd);

  strcpy(path_cmd, "");

  for (i = 0; i < strlen(str); ++i){
    if (strncmp(&str[i], ";", 1) == 0) {
      strcat(path_cmd, cmd);
      if (is_exist(path_cmd)) {
        break;
      } else {
        strcpy(path_cmd, "");
      }
    } else {
      strncat(path_cmd, &str[i], 1);
    }
  }

  return path_cmd;
}
