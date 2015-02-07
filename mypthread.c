#include "mypthread.h"
#include <sys/ucontext.h>
#include <unistd.h>       // usleep(..)
#include <stdio.h>        // printf(..)
#include <stdlib.h>       // atoi(..)
#include "mypthread-system-override.h"
ucontext_t coroutine_main;

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg){
	printf("start initializing a thread\n");
	sleep(1);
	getcontext(&((*thread).ctx));
    (*thread).ctx.uc_stack.ss_sp = (*thread).st;
    (*thread).ctx.uc_stack.ss_size = sizeof((*thread).st);
    // (*thread).ctx.uc_link = &((thread + 1) -> ctx);
    makecontext(&((*thread).ctx), &(*start_routine), 1, arg);
	printf("finish initializing\n");

	return 0;
}

void mypthread_exit(void *retval){

}

int mypthread_yield(void){
	printf("start yielding next thread\n");
	// ucontext_t u;
	// getcontext(&u);
	// swapcontext(&u, u.uc_link);
	setcontext(&coroutine_main);
	// swapcontext(&u, &coroutine_main);
	printf("finish yielding\n");

	return 0;
}

int mypthread_join(mypthread_t thread, void **retval){
	printf("start joining a thread\n");
	sleep(1);
	// ucontext_t u;
	// swapcontext(&u, &(thread.ctx));
	// getcontext(&coroutine_main);
	// setcontext(&(thread.ctx));
	swapcontext(&coroutine_main, &(thread.ctx));
	// setcontext(thread.ctx.uc_link);
	// printf("finish joining\n");

	return 0;
}
