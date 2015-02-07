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

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg){
	printf("start initializing a thread\n");
	// sleep(1);
	getcontext(&((*thread).ctx));
    (*thread).ctx.uc_stack.ss_sp = (*thread).st;
    (*thread).ctx.uc_stack.ss_size = sizeof((*thread).st);
    // (*thread).ctx.uc_link = &((thread + 1) -> ctx);
    makecontext(&((*thread).ctx), &(*start_routine), 1, arg);
	printf("finish initializing\n");

	return 0;
}

void mypthread_exit(void *retval){
	setcontext(&coroutine_main);
}

int mypthread_yield(void){
	printf("start yielding next thread\n");
	// ucontext_t u;
	// getcontext(&u);
	// swapcontext(&u, u.uc_link);
	// setcontext(&coroutine_main);
	// if(first){
		if(thread_id <= 30){
			if(first){
				thread_id++;
				swapcontext(&coroutine_sub[thread_id], &coroutine_main);	
			}
			else{
				thread_id++;
				swapcontext(&coroutine_sub[thread_id], &coroutine_sub[thread_id + 1]);
			}
		}
		else if(thread_id == 31){
			thread_id = 0;
			first = 0;
			swapcontext(&coroutine_sub[thread_id + 32], &coroutine_sub[thread_id + 1]);
		}
		// else{
		// 	thread_id--;
		// 	swapcontext(&coroutine_sub[thread_id + 1], &coroutine_sub[thread_id]);
		// }

	// 		first = 0;
	// 		swapcontext(&coroutine_sub[thread_id], &coroutine_sub[thread_id - 1]);
	// 	}
	// }
	// else{
	// 	thread_id--;

	// }
	
	// swapcontext(&u, &coroutine_main);
	printf("finish yielding\n");

	return 0;
}

int mypthread_join(mypthread_t thread, void **retval){
	printf("start joining a thread\n");
	// sleep(1);
	// ucontext_t u;
	// swapcontext(&u, &(thread.ctx));
	// getcontext(&coroutine_main);
	// setcontext(&(thread.ctx));
	swapcontext(&coroutine_main, &(thread.ctx));
	// setcontext(thread.ctx.uc_link);
	// printf("finish joining\n");

	return 0;
}
