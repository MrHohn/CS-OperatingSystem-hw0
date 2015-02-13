#ifndef H_ANALYZE_CACHE
#define H_ANALYZE_CACHE

// Your header code goes here, between #define and #endif
__u64 gettime(void);
int get_cache_linesize(char array[]);
int get_cache_size(int array[]);
double get_cache_misspenalty(void);
int cache_line_helper(char array[], int len, int K);
int cache_size_helper(int array[], int range);

#endif

