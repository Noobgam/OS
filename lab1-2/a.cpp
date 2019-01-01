#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <new>

#define PIPE_READ 0
#define PIPE_WRITE 1

int main() {
  int pipe1[2];
  int pipe2[2];
  int nChild;

  if (pipe(pipe1) < 0) {
    perror("allocating pipe for child input redirect");
    return -1;
  }
  if (pipe(pipe2) < 0) {
    close(pipe1[PIPE_READ]);
    close(pipe1[PIPE_WRITE]);
    perror("allocating pipe for child output redirect");
    return -1;
  }

  nChild = fork();
  if (0 == nChild) {
    // child continues here

    // redirect stdin
    if (dup2(pipe1[PIPE_READ], STDIN_FILENO) == -1) {
      exit(errno);
    }

    // redirect stdout
    if (dup2(pipe2[PIPE_WRITE], STDOUT_FILENO) == -1) {
      exit(errno);
    }

    // all these are for use by parent only
    close(pipe1[PIPE_READ]);
    close(pipe1[PIPE_WRITE]);
    close(pipe2[PIPE_READ]);
    close(pipe2[PIPE_WRITE]); 

    int n, m;
    scanf("%d%d", &n, &m);
    double answer = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int val;
            scanf("%d", &val);
            answer += val;
        }
    }
    printf("%lf", answer / (n * m));
    exit(nChild);
  } else if (nChild > 0) {
    // parent continues here

    // close unused file descriptors, these are for child only
    close(pipe1[PIPE_READ]);
    close(pipe2[PIPE_WRITE]); 

    // Include error check here
    int n, m;
    scanf("%d%d", &n, &m);
    int** a = new int*[n];
    for (int i = 0; i < n; ++i) {
        a[i] = new int[m];
        for (int j = 0; j < m; ++j) {
            scanf("%d", &a[i][j]);
        }
    }
    FILE* write_file = fdopen(pipe1[PIPE_WRITE], "w");
    fprintf(write_file, "%d %d\n", n, m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            fprintf(write_file, "%d%c", a[i][j], " \n"[j + 1 == m]);
        }
    }
    fclose(write_file);
    FILE* read_file = fdopen(pipe2[PIPE_READ], "r");
    double ans;
    fscanf(read_file, "%lf", &ans);
    fclose(read_file);
    printf(" < %lf\n", ans);
    return 0;
  } else {
    // failed to create child
    close(pipe1[PIPE_READ]);
    close(pipe1[PIPE_WRITE]);
    close(pipe2[PIPE_READ]);
    close(pipe2[PIPE_WRITE]);
  }
  return nChild;
}
