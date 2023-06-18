#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define THREAD_COUNT 8  // Number of threads to use
#define TARGET_COUNT 1000000000000ULL  // 1 trillion

typedef struct {
    uint64_t start;
    uint64_t end;
    uint64_t count;
} ThreadData;

void* countRange(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    uint64_t count = 0;

    // Perform counting using simple loop
    uint64_t counter = data->start;
    uint64_t increment = 8ULL;

    while (counter <= data->end && counter <= TARGET_COUNT) {
        ++count;
        counter += increment;
    }

    data->count = count;
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    ThreadData threadData[THREAD_COUNT];
    uint64_t totalCount = 0ULL;

    uint64_t rangeSize = TARGET_COUNT / THREAD_COUNT;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threadData[i].start = i * rangeSize + 1ULL;
        threadData[i].end = (i + 1) * rangeSize;
        pthread_create(&threads[i], NULL, countRange, (void*)&threadData[i]);
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i], NULL);
        totalCount += threadData[i].count;
    }

    printf("Total count: %llu\n", totalCount);
    return 0;
}
