#include <stdio.h>
#include <stdlib.h> 
#include <linux/types.h>
#include <string.h>
#include <stdbool.h>
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

int get_cache_linesize(char array[])
{
    int i, K, i_test = 0;
    double diff, access_time, access_cost[20];
    // double average_access_cost[10];

    cache_line_helper(array, NUMBER*4, 1);

    for(K = 1;K < 8096;K *= 2) 
    {
        cache_line_helper(array, NUMBER*4, K);
        diff = end - begin;
        access_cost[i_test] = diff;
        access_time = NUMBER / K;
        // average_access_cost[i_test] = diff / access_time;
        // printf("When K = %d, Average access time: %f us\n", K, average_access_cost[i_test]);
        printf("when K = %8d,access %8d times,cost %8llu us\n", K,NUMBER/K,end - begin);     
        i_test++;
    }
    
    int cachelinesize = 0, index = 1;
    double divide0, divide1, divide2;
    for(i = 2; i < 1024; i *=2){
        divide0 = access_cost[index] / access_cost[index-1];
        divide1 = access_cost[index+2] / access_cost[index+1];
        divide2 = access_cost[index+3] / access_cost[index+2];
        // printf("%d %f %f %f\n", i, divide0, divide1, divide2);
        if((divide1 < 0.7) && (divide2 < 0.65) && (divide0 > 0.85)){
            cachelinesize = i;
        }
        index++;
    }
    // printf("Cache Block/Line Size: %d B\n", cachelinesize);

    return cachelinesize;

}

int cache_line_helper(char array[], int len, int K)
{    
    int i,j;
    // char a;
    begin = gettime();
    for(i = 0;i < len; i += K)
    {
        array[i] = 's';
    }
    end = gettime();

    return 0;

}

int get_cache_size(int array[])
{
    int ranges[14];
    double rates[14];
    int index_cache_size = 0;
    int range;
    /*
    -----------test the size of L2 cache------------------------------
    -----------by changing the range of memory we access---------------
    */
    for(range = 1024; range <= NUMBER/8; range *= 2)
    {
        int pre_cost = end - begin; 
            
        cache_size_helper(array, range);
        end = gettime();
        
        int cost = end - begin;
        double change_rate= (double) abs(cost - pre_cost) / pre_cost; 

        printf("when range = %10d B, cost %10d us, increase rate: %f\n", range*4, cost, change_rate);

        ranges[index_cache_size] = range*4;
        rates[index_cache_size] = change_rate; 
        index_cache_size++; 
    }
    
    int cachesize[3]; // Track the sizes for L1, L2, L3 cache
    int level = 0;

    int i;
    for(i = 1; i < 14; i++) 
    {
        if (rates[i + 1] >= 1.5*rates[i] && rates[i + 1] >= 1.5*rates[i + 2])
        {
            // If the change rate of cost is apparantly larger than its neighbor's, 
            // then there must be a capacity miss, so we fetch the result
            cachesize[level++] = ranges[i];
        }
    }    

    return cachesize[1]/1024;
}

int cache_size_helper(int array[], int range)
{
    int i;
    int lenmod = range -1;
    int times = 64*SIZE_1MB*8;
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

double get_cache_misspenalty(void)
{ 
    int i, j;
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
            array_missPenalty[(i * 32) & (NUMBER-1)]++;
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

    return penalty[0];
}

int main(int argc, char *argv[])
{
    /*--------below is the code for testing cache line size---------*/

    int i, j, K;
    char *array_cacheline = NULL;
    array_cacheline = malloc(NUMBER*4*sizeof(char));
    
    if(array_cacheline == NULL)
    {
        printf("malloc space for array failed \n");
        return -1;   
    }

    int cachelinesize, timecacheline;

    for(timecacheline = 0; timecacheline < 10; timecacheline++)
    {
        cachelinesize = get_cache_linesize(array_cacheline);
        if(cachelinesize != 0)break;
    }

    printf("Cache Block/Line Size: %d B\n", cachelinesize);

    free(array_cacheline);
    

    /*--------the code for testing L2 cache size---------*/

    int cachesize = 0;
    bool done = false;
    int *array_cachesize = NULL;
    array_cachesize = malloc(NUMBER*sizeof(int));
    
    if(array_cachesize == NULL)
    {
        printf("malloc space for array failed \n");
        return -1;
    }

    for(timecacheline = 0; timecacheline < 3; timecacheline++)
    {
         cachesize = get_cache_size(array_cachesize);
        // If the result is not resonable, execute the test again.
        if (cachesize >= 128 && cachesize <= 1024)break;
        cachesize = 0;
    }

    printf("Cache Size: %d KB\n", cachesize);
 
    free(array_cachesize);
        
    /*--------Last part is the code for testing miss penalty---------*/
    
    double penalty = get_cache_misspenalty();
    
    printf("Cache Miss Penalty: %f us\n", penalty);

    return 0;
}