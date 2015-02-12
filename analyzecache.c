#include <stdio.h>
#include <stdlib.h> 
#include <linux/types.h>
#include <string.h>
#include "analyzecache.h"

#define SIZE_1KB (1024)
#define SIZE_1MB (1024*1024)
#define SIZE_512KB (512*1024)

#define NUMBER 64*SIZE_1MB 
#define MILLION 1000000
__u64 begin;
__u64 end;
__u64 gettime()
{
        struct timeval tv;
        gettimeofday(&tv,NULL);
        return ((__u64)(tv.tv_sec))*MILLION+tv.tv_usec;
}

int test_cache_linesize(int array[],int len,int K)
{
    int i,j;
    for(j = 0; j < 16; j += 1){
        for( i = 0;i<len/32;i += K)
        {
            array[i]++;
        }   
    }

    return 0;

}

int test_cache_capacity(int array[],int range)
{
    int i,j;
    int lenmod = range -1;
    int times = 64*SIZE_1MB*8;
    int flag = range / 16 -1;
    for(i = 0;i<=flag;i++)
    {
        array[(i*16) & (lenmod)]++;/*16 means linesize/sizeof(int) = 16*/
    }
    begin = gettime();
    for(i = 0;i<times;i++)
    {
        array[(i*16) & (lenmod)]++;/*16 means linesize/sizeof(int) = 16*/
    }
    return 0;
}


int test_cache_missPenalty(int array[], int size, int K){

}

int test_cache()
{
    int *array = NULL;
    array = malloc(NUMBER*8*sizeof(int));
    // __u64 begin ;
    // __u64 end;
    int i;
    int K;
    int range ;
    int size;
    if(array == NULL)
    {
         printf("malloc space for array failed \n");
         return -1;
    }
    
    for(i = 0;i<NUMBER;i++)
    {
        array[i] = i;
    }
    test_cache_linesize(array,NUMBER,1);
   
   printf("---------test cache linesize-------------------------------------------\n");
   printf("---------by chagning the interval we access the array----------\n");   
    for(K = 1;K < 1024;K *= 2) 
    {
         begin = gettime();
         test_cache_linesize(array,NUMBER*8,K);
         end = gettime();
         printf("when K = %8d,multiply %8d times,cost %8llu us,average cost %llu us\n",
         K,NUMBER/K,end - begin,(end-begin)/(NUMBER/K));
         // if(K == 1)
         // {
         //             begin = gettime();
         //             test_cache_linesize(array,NUMBER,K);
         //             end = gettime();
         //             printf("when K = %8d,multiply %8d times,cost %8llu us,average cost %llu us\n",
         //                     K,NUMBER/K,end - begin,(end-begin)/(NUMBER/K));
         // }
         
     } 
    
    printf("-----------test the size of L2 cache-----------------------\n");
    printf("-----------by changing the range of memory we access-----------------------\n");
    for(range = 1024;range <= NUMBER/8;range *= 2)
    {
         // begin =gettime();
         test_cache_capacity(array,range);
         end = gettime();
         printf("when range = %10d B,cost %14llu us\n",
         range*4,end-begin);
         // if(range == 2*SIZE_1MB/sizeof(int))
         // {
         //      begin =gettime();
         //      test_cache_capacity(array,3*SIZE_1MB/sizeof(int));
         //      end = gettime();
         //      printf("when range = %10d,cost %14llu us\n",
         //               3*SIZE_1MB,end-begin);
         // }
                
    }

    free(array);
    return 0;
        
}

int main(int argc, char *argv[])
{
    test_cache();

    int i, j;
   __u64 times_missPenalty = 3000000;
    __u64 loop = 10;
    double penalty[loop];

    for(j = 0; j < loop; j++){
        int *array_missPenalty = NULL;
        array_missPenalty = malloc(64*1024*1024*sizeof(int));
        begin = gettime();
        for(i = 0; i < times_missPenalty; i++)
        {
            /*set the interval to 64 to prevent prefetch optimization based on locality*/
            array_missPenalty[(i * 64) & (NUMBER-1)]++;
        }
        end = gettime();
        // printf("cost = %10llu us\n", end - begin);
        double diff = end - begin;
        double access = times_missPenalty;
        penalty[j] = diff / access;
        // printf("miss penalty= %10f ns\n", penalty[j]);
        free(array_missPenalty);
    }

    for(i = 0; i < loop; i++){
        penalty[0] += penalty[i];    
    }
    penalty[0] /= loop;
    printf("Cache Miss Penalty: %f us\n", penalty[0]);

    return 0;
}
