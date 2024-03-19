#include <iostream>
#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <omp.h>

using namespace std;

#define NUM_BUCKETS 10
#define NUM_THREADS 4

vector<int> arr;
vector<int> buckets[NUM_BUCKETS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_sort(void* arg) {
    int tid = *((int*)arg);
    int start = tid * (arr.size() / NUM_THREADS);
    int end = (tid == NUM_THREADS - 1) ? arr.size() : (tid + 1) * (arr.size() / NUM_THREADS);

    for (int i = start; i < end; ++i) {
        int bucket_index = arr[i] / (arr.size() / NUM_BUCKETS);
        pthread_mutex_lock(&mutex);
        buckets[bucket_index].push_back(arr[i]);
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void parallel_bucket_sort_pthreads() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_sort, (void*)&thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NUM_BUCKETS; ++i) {
        sort(buckets[i].begin(), buckets[i].end());
    }

    int index = 0;
    for (int i = 0; i < NUM_BUCKETS; ++i) {
        for (int j = 0; j < buckets[i].size(); ++j) {
            arr[index++] = buckets[i][j];
        }
    }
}

void parallel_bucket_sort_openmp() {
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid = omp_get_thread_num();
        int start = tid * (arr.size() / NUM_THREADS);
        int end = (tid == NUM_THREADS - 1) ? arr.size() : (tid + 1) * (arr.size() / NUM_THREADS);

        for (int i = start; i < end; ++i) {
            int bucket_index = arr[i] / (arr.size() / NUM_BUCKETS);
            #pragma omp critical
            buckets[bucket_index].push_back(arr[i]);
        }
    }

    for (int i = 0; i < NUM_BUCKETS; ++i) {
        sort(buckets[i].begin(), buckets[i].end());
    }

    int index = 0;
    for (int i = 0; i < NUM_BUCKETS; ++i) {
        for (int j = 0; j < buckets[i].size(); ++j) {
            arr[index++] = buckets[i][j];
        }
    }
}