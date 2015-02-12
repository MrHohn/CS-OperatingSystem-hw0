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
    int a;
    begin = gettime();
    // for(j = 0; j < 16; j += 1){
        for(i = 0;i < len; i += K)
        {
            array[i]++;
            // a = array[i];
            // int b = a + a;
        }   
    // }
    end = gettime();
    // double diff = end - begin;
    // double access = len / K;
    // double average_access_time = diff / access;
    // printf("When K = %d, Average access time: %f us\n", K, average_access_time);

    return 0;

}

int test_cache_capacity(int array[],int range)
{
    int i;
    int lenmod = range -1;
    int times = 64*SIZE_1MB;
    /*the flag is indicating the range size*/
    int flag = range / 16 -1;
    /*loop the array one time first in order to put the data into cache*/
    for(i = 0;i<=flag;i++)
    {
        array[(i*16) & (lenmod)]++;/*16 means linesize/sizeof(int) = 16*/
    }
    begin = gettime();
    for(i = 0;i<times;i++)
    {
        array[(i*16) & (lenmod)]++;/*16 means linesize/sizeof(int) = 16*/
    }
    end = gettime();
    return 0;
}


int test_cache_missPenalty(int array[], int size, int K){

}


int main(int argc, char *argv[])
{
    
    int *array = NULL;
    array = malloc(NUMBER*sizeof(int));

    int i, j;
    int K;
    int i_test = 0;
    int range;
    int size;
    double diff_cache_line;
    double access_cache_line;
    double average_access_time[10];

    if(array == NULL)
    {
        printf("malloc space for array failed \n");
        return -1;
    }
    
    test_cache_linesize(array,NUMBER,1);
    /*
    printf("---------test cache linesize-------------------------------------------\n");
    printf("---------by chagning the interval we access the array----------\n");   
    */
    for(K = 1;K < 1024;K *= 2) 
    {
        test_cache_linesize(array,NUMBER,K);
        diff_cache_line = end - begin;
        access_cache_line = NUMBER / K;
        average_access_time[i_test] = diff_cache_line / access_cache_line;
        printf("When K = %d, Average access time: %f us\n", K, average_access_time[i_test]);
        printf("when K = %8d,access %8d times,cost %8llu us\n", K,NUMBER/K,end - begin);     
        i_test++;
    } 

    double rate_cache_line[9];
    for(i = 0; i < 9; i++){
        rate_cache_line[i] = (average_access_time[i+1] - average_access_time[i]) / average_access_time[i];
        printf("%f\n", rate_cache_line[i]);
    }
    int cache_line_size = 0;
    int index = 0;
    for(i = 1; i < 128; i *=2){
        if(abs(rate_cache_line[index+1]) < 0.2 && abs(rate_cache_line[index+2]) < 0.2 && rate_cache_line[index] > 0.3){
            cache_line_size = i * 4;
        }
        index++;
    }
    printf("Cache Block/Line Size: %d B\n", cache_line_size);






    /*
    printf("-----------test the size of L2 cache-----------------------\n");
    printf("-----------by changing the range of memory we access-----------------------\n");
    */
    for(range = 1024;range <= NUMBER/8;range *= 2)
    {
        test_cache_capacity(array,range);
        /*the range would be timed 4*/
        /*it is the number of elements in an int array*/
        // printf("when range = %10d B,cost %14llu us\n", range*4,end-begin);
                
    }

    free(array);
        
    /*--------Last part is the code for testing miss penalty---------*/
    /*set times of access to a number smaller than arraysize/interval */
    /*to prevent out of range*/
   __u64 times_missPenalty = 3000000;
    __u64 loop = 10;
    double penalty[loop];
    /*test the miss penalty for 'loop' times*/
    for(j = 0; j < loop; j++){
        /*redefined the array to insure miss occurred everytime*/
        int *array_missPenalty = NULL;
        /*init a 256MB size array*/
        array_missPenalty = malloc(64*1024*1024*sizeof(int));
        begin = gettime();
        /*we do not do the outter loop for testing penalty*/
        /*beacause the processor seem to have some optimizations for it*/
        for(i = 0; i < times_missPenalty; i++)
        {
            /*set the interval to 32 to prevent prefetch optimization based on locality*/
            array_missPenalty[(i * 16) & (NUMBER-1)]++;
        }
        end = gettime();
        // printf("cost = %10llu us\n", end - begin);
        double diff = end - begin;
        double access = times_missPenalty;
        penalty[j] = diff / access;
        // printf("miss penalty= %10f ns\n", penalty[j]);
        free(array_missPenalty);
    }
    /*calculate the average result*/
    for(i = 0; i < loop; i++){
        penalty[0] += penalty[i];    
    }
    penalty[0] /= loop;
    printf("Cache Miss Penalty: %f us\n", penalty[0]);

    return 0;
}