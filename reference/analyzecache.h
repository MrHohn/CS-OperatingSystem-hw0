#ifndef H_ANALYZE_CACHE
#define H_ANALYZE_CACHE

// Your header code goes here, between #define and #endif
//__u64 gettime(void);
int test_cache_linesize(int array[],int len,int K);
int test_cache_capacity(int array[],int cap);
int test_cache_missPenalty(int array[], int size, int K);
// int test_cache_associative(int array[],int size,int K);
int test_cache(void);


#endif

