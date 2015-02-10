#include "mypthread.h"
#include <sys/ucontext.h>
#include <unistd.h>       // usleep(..)
#include <stdio.h>        // printf(..)
#include <stdlib.h>       // atoi(..)
#include "mypthread-system-override.h"
ucontext_t coroutine_main;
ucontext_t coroutine_sub[32];
int thread_id = 0;
int first = 1;
int numOfThread = 0;
int currentId = 0;

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg){
	// printf("start initializing a thread\n");
	// sleep(1);
	getcontext(&((*thread).ctx));
    (*thread).ctx.uc_stack.ss_sp = (*thread).st;
    (*thread).ctx.uc_stack.ss_size = sizeof((*thread).st);
    // (*thread).ctx.uc_link = &((thread + 1) -> ctx);
    makecontext(&((*thread).ctx), &(*start_routine), 1, arg);
    numOfThread++;
	// printf("finish initializing\n");

	return 0;
}

void mypthread_exit(void *retval){
	// printf("%d\n", numOfThread);
	setcontext(&coroutine_main);
}

int mypthread_yield(void){
	// printf("start yielding next thread\n");
	// sleep(1);
	if((thread_id <= numOfThread - 2) || (currentId <= numOfThread - 1)){
		if(first){
			thread_id++;
			if(thread_id != currentId)thread_id--;
			if(currentId == 1){
				// printf("first one %d\n", currentId);
				swapcontext(&coroutine_sub[currentId], &coroutine_main);
			}
			else{
				// printf("backing %d\n", currentId);
				currentId--;
				swapcontext(&coroutine_sub[currentId + 1], &coroutine_sub[currentId]);
			}
		}
		else{
			thread_id++;
			// printf("not first loop %d\n", thread_id);
			swapcontext(&coroutine_sub[thread_id], &coroutine_sub[thread_id + 1]);
		}
	}
	else if(thread_id == numOfThread - 1){
		// printf("num of thread: %d\n", numOfThread);
		// printf("reach the last thread %d\n", thread_id + 1);
		thread_id = 0;
		first = 0;
		currentId = numOfThread;
		swapcontext(&coroutine_sub[thread_id + numOfThread], &coroutine_sub[thread_id + 1]);
	}


	// printf("finish yielding\n");

	return 0;
}

int mypthread_join(mypthread_t thread, void **retval){
	// printf("start joining a thread\n");
	// sleep(1);
	currentId = 1 + thread_id;
	swapcontext(&coroutine_main, &(thread.ctx));

	return 0;
}


