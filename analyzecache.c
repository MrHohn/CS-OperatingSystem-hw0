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

__u64 gettime()
{
       struct timeval tv;
        gettimeofday(&tv,NULL);
        return ((__u64)(tv.tv_sec))*MILLION+tv.tv_usec;
}

int test_cache_linesize(int array[],int len,int K)
{
 int i,j;
    for( i = 0;i<len;i += K)
    {
          // array[i] *=3;
        // j += array[i];
        array[i]++;
    }

    return 0;

}

int test_cache_capacity(int array[],int cap)
{
    int i,j;
    int lenmod = cap -1;
    int times = 64*SIZE_1MB;
     for(i = 0;i<times;i++)
     {
        if(i == 1)begin = gettime();
         array[(i*16) & (lenmod)]++;/*16 means linesize/sizeof(int) = 16*/
         // j += array[(i*16) & (lenmod)];/*16 means linesize/sizeof(int) = 16*/
     }
     return 0;
}

// int test_cache_associative(int array[],int size,int K)
// {
//     int i;
//      int cur =0;
//      __u64 begin;
//      __u64 end;
//      begin =gettime();
//      for( i = 0;i<SIZE_1MB;i++)
//      {
//          array[cur]++;
//          cur += K;
//          if(cur >= size)
//          cur = 0;
//      }
//      end = gettime();
    
//      printf("when size = %10d, K = %10d : test_cache_associative cost %14llu us\n",
//      size,K ,end-begin);
//      return 0;
// }

int test_cache_missPenalty(int array[], int size, int K){

}

int test_cache()
{
    int *array = NULL;
    array = malloc(NUMBER*sizeof(int));
    // __u64 begin ;
    __u64 end;
    int i;
    int K;
    int cap ;
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
   
   printf("---------test cache linesize-------------------------------------------\n");
   printf("---------by chagning the interval we access the array----------\n");   
    for(K = 1;K < 1024;K *= 2) 
    {
         begin = gettime();
         test_cache_linesize(array,NUMBER,K);
         end = gettime();
         printf("when K = %8d,multiply %8d times,cost %8llu us,average cost %llu us\n",
         K,NUMBER/K,end - begin,(end-begin)/(NUMBER/K));
         // if(K == 1)
         // {
         //             begin = gettime();
         //             test_cache_linesize(array,NUMBER,K);
         //             end = gettime();
         //             printf("when K = %10d,multiply %10d times,cost %14llu us,average cost %llu us\n",
         //                     K,NUMBER/K,end - begin,(end-begin)/(NUMBER/K));
         // }
         
     } 
    
    printf("-----------test the size of L2 cache-----------------------\n");
    printf("-----------by changing the range of memory we access-----------------------\n");
    for(cap = 1024;cap <= NUMBER;cap *= 2)
    {
         // begin =gettime();
         test_cache_capacity(array,cap);
         end = gettime();
         printf("when range = %10d B,cost %14llu us\n",
         cap*4,end-begin);
         // if(cap == 2*SIZE_1MB/sizeof(int))
         // {
         //      begin =gettime();
         //      test_cache_capacity(array,3*SIZE_1MB/sizeof(int));
         //      end = gettime();
         //      printf("when cap = %10d,cost %14llu us\n",
         //               3*SIZE_1MB,end-begin);
         // }
                
    }
    // printf("-----------test cache associative ---------------------------------------\n");

    // for(size = 1*SIZE_1MB;size >= 4*SIZE_1KB;size /= 2)
    // { 
    //      for(K = 64;K <= 576 ;K += 64) 
    //      {
    //           test_cache_associative(array,size,K); 
    //      } 
    // }

    printf("-----------test miss penalty ---------------------------------------\n");


    free(array);
    return 0;
        
}

int main(int argc, char *argv[])
{
    test_cache();

    // int *array1 = NULL;
    // array1 = malloc(2*1024*1024*sizeof(int));
    // __u64 end;
    // // int *array2 = NULL;
    // // array2 = malloc(128*1024*sizeof(int));
    // int i;
    // int lenmod1 = 2*1024*1024/4 -1;
    // int lenmod2 = 256*1024/4 -1;
    // int lenmod3 = 32*1024/4 -1;
    // int times1 = 2*1024*1024/4;
    // int times2 = 256*1024/4;

    // //initialize array(bring the data in to L3 L2 L1 cache)
    // for(i = lenmod1; i >= 0; i--)
    // {
    //     array1[i]++;
    // }
    // printf("Finish initialize");
    // //array size 524288
    // // if just one loop, not more than 32768 times access
    // printf("-----------access the data already inside the L2 cache-----------------------\n");
    // begin = gettime();
    // for(i = 0; i <= 100000000; i++){
    //      array1[(i*16) & (lenmod2)]++;        
    // }
    // end = gettime();
    // __u64 cost1 = end-begin;
    // printf("cost %10llu us without miss\n", cost1);

    // //initialize array(bring the data in to L3 L2 L1 cache)
    // for(i = 0; i <= lenmod1; i++)
    // {
    //     array1[i]++;/*16 means linesize/sizeof(int) = 16*/
    // }

    // printf("-----------access the data not in the L2 cache-----------------------\n");
    // begin = gettime();
    // for(i = 0; i <= 100000000; i++){
    //      array1[(i*16) & (lenmod1)]++;          
    // }
    // end = gettime();
    // __u64 cost2 = end-begin;
    // printf("cost %10llu us with all l2 miss\n", cost2);
    // printf("difference = %10llu \n", cost2 - cost1);

    // //initialize array(bring the data in to cache)
    // for(i = 0;i < times2; i++)
    // {
    //     array2[i*16]++;
    // }

    // printf("-----------test within L2 cache capacity-----------------------\n");
    // begin = gettime();
    // for(i = 0; i < 2*1024*1024/4; i++){
    //     array[(i*16) & (lenmod2)]++;        
    // }
    // end = gettime();
    // printf("cost %10llu us without miss\n", end - begin);
    // free(array1);
    return 0;
}