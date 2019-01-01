#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <new>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main() {
    int* shared;
    double* result;
    int nChild;
    {
        int n, m;
        scanf("%d%d", &n, &m);
        shared = (int*) mmap(NULL, sizeof(int) * (2 + n * m), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (shared == MAP_FAILED) {
            fprintf(stderr, "Memory could not be mmaped\n");
            return 1;    
        }
        result = (double*) mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (result == MAP_FAILED) {
            fprintf(stderr, "Memory could not be mmaped\n");
            return 1;    
        }
        shared[0] = n;
        shared[1] = m;
        int id = 2;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                scanf("%d", &shared[id++]);
            }
        }
    }

    nChild = fork();
    if (nChild < 0) {
        fprintf(stderr, "Couldnt fork\n");
        return 1;
    }
    if (0 == nChild) {
        double answer = 0;
        int n = shared[0];
        int m = shared[1];
        int id = 2;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                answer += shared[id++];
            }
        }
        result[0] = answer / (n * m);
        exit(0);
    }
    int statusChild;
    waitpid(nChild, &statusChild, 0);
    if (statusChild != 0) {
        fprintf(stderr, "Calculation failed\n");
        return 1;
    }
    printf(" < %d\n", WEXITSTATUS(statusChild));
    printf(" < %lf\n", result[0]);
    return 0;
}
