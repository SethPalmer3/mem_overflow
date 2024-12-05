#include <asm-generic/errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MEM_LIMIT 1000000 // in bytes
#define HARMLESS 1

void parent_handler(int signum) {
  int status;
  pid_t pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if (WIFEXITED(status)) {
      printf("Child exited\n");
    } else if (WIFSIGNALED(status)) {
      printf("Child terminated by signal %d\n", WTERMSIG(status));
    }
  }
  printf("Parent handler called.\n");
}

int main(int argc, char *argv[]) {
  // Create sub process
  int fd[2]; // fd[0] = read end, fd[1] = write end
  if (pipe(fd) < 0) {
    printf("Pipe creation failed.\n");
    return EXIT_FAILURE;
  }
  pid_t pid = fork();
  if (pid < 0) {
    printf("Fork failed.\n");
    return EXIT_FAILURE;
  } else if (pid == 0) {
    //  - Sub process -
    //  allocate memory
    if (!HARMLESS) {
      int *t = (int *)malloc(MEM_LIMIT);
      *t = 15;
    }
    //  send signal to parent process
    if (kill(getppid(), SIGUSR1) == -1) {
      perror("Failed to send signal\n");
      return EXIT_FAILURE;
    }
    printf("Sent signal to parent.\n");
    return EXIT_SUCCESS;
  } else {
    int total_memory = 0;
    //  - Parent process -
    //  check for child signal
    signal(SIGUSR1, parent_handler);
    while (1)
      ;

    //  increase running memory total
    total_memory += MEM_LIMIT;
    //  if child dies, run another process with recorded level
  }
  return EXIT_SUCCESS;
}
