#include <cstdio>
#include <time.h>
#include <random>
#include <pthread.h>
#include <new>

long long calculateSum(int* begin, int* end) {
    long long result = 0;
    while (begin != end)
        result += *begin++;
    return result;
}

void* thread_helper(void* args) {
    int** data = (int**) args;
    int* begin = data[0];
    int* end = data[1];
    long long res = calculateSum(begin, end);
    return (void*)res;
}

int main(int argc, char** argv) {
    int threads = 2;
    if (argc > 0) {
        threads = atoi(argv[0]);
        if (threads == 0) {
            threads = 2;
        }
    }
    int K, N;
    printf("Print array size and amount:\n");
    scanf("%d %d", &N, &K);
    printf("Randomize data - 0, read data from input 1\n");
    int filler = 0;
    scanf("%d", &filler);
    int **arr = new int*[K];
    srand(time(NULL));
    for (int i = 0; i < K; ++i) {
        arr[i] = new int[N];
        for (int j = 0; j < N; ++j) {
            if (filler == 0) {
                arr[i][j] = rand() % 1000;
            } else {
                scanf("%d", &arr[i][j]);
            }
        }
    }
    int processed = 0;
    long long totalsum = 0;
    for (;processed < K;) {
        int status;
        long long retvalue[threads];
        pthread_t threadContainer[threads];
        int** data[threads];
        int threadCount = 0;
        for (threadCount = 0; threadCount < threads && processed + threadCount < K; ++threadCount) {
            fprintf(stderr, "%d\n", threadCount);
            data[threadCount] = (int**)malloc(sizeof(int*) * 2);
            data[threadCount][0] = arr[processed + threadCount];
            data[threadCount][1] = arr[processed + threadCount] + N;
            status = pthread_create(&threadContainer[threadCount], NULL, &thread_helper, data[threadCount]);
            if (status != 0) {
                printf("Can't create thread, status %d", status);
                exit(1);
            }
        }
        for (int i = 0; i < threadCount; ++i) {
            status = pthread_join(threadContainer[i], (void**)&retvalue[i]);
            if (status != 0) {
                printf("Can't join thread, status %d", status);
                exit(1);
            }
            totalsum += retvalue[i];
            free(data[i]);
        }
        processed += threadCount;
        fprintf(stderr, "Processed: %d\n", processed);
    }
    printf("Total sum: %lld\n", totalsum);
    for (int i = 0; i < K; ++i) {
        delete[] arr[i];
    }
    delete[] arr;

    return 0;
}

