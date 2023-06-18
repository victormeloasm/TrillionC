#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define THREAD_COUNT 32
#define TARGET_COUNT 1000000000000ULL // 1 trillion

typedef struct {
    uint64_t start;
    uint64_t end;
    uint64_t count;
} ThreadData;

void* countRange(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    uint64_t count = 0;

    // Perform counting within the assigned range, up to 1 trillion
    uint64_t counter = data->start;
    uint64_t increment = 8ULL;
    uint64_t targetCount = data->end;

    // Unroll the loop to process multiple iterations at once
    while (counter <= targetCount - 8) {
        count += 8;
        counter += increment * 8;
    }

    // Process the remaining iterations
    while (counter <= targetCount) {
        count += 1;
        counter += increment;
    }

    data->count = count;
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    ThreadData threadData[THREAD_COUNT];
    uint64_t totalCount = 0ULL;

    uint64_t rangeSize = (TARGET_COUNT + THREAD_COUNT - 1) / THREAD_COUNT;
    uint64_t remainingCount = TARGET_COUNT % THREAD_COUNT;
    uint64_t currentStart = 1ULL;

    for (int i = 0; i < THREAD_COUNT; ++i) {
        threadData[i].start = currentStart;
        threadData[i].end = currentStart + rangeSize - 1ULL;

        // Distribute the remaining count among the first few threads
        if (remainingCount > 0) {
            threadData[i].end++;
            remainingCount--;
        }

        pthread_create(&threads[i], NULL, countRange, (void*)&threadData[i]);

        currentStart = threadData[i].end + 1ULL;
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i], NULL);
        totalCount += threadData[i].count;
    }

    printf("Total count: %llu\n", totalCount);
    return 0;
}
