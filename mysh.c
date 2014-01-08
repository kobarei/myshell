#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void left_prompt();

int main(int argc, char const *argv[]) {
  char str[256];
  left_prompt();
  while(read(0, str, 256)){
    left_prompt();

    //
    // EXIT if 'exit' input
    //
    if (strcmp(str, "exit\n") == 0)
      break;
  }
  return 0;
}

//
// output left prompt
//
void left_prompt() {
  write(1, "mysh> ", 6);
}
