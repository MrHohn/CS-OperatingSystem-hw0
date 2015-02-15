#include "mypthread.h"
#include <sys/ucontext.h>
#include <unistd.h>       // usleep(..)
#include <stdio.h>        // printf(..)
#include <stdlib.h>       // atoi(..)
#include "mypthread-system-override.h"
ucontext_t coroutine_main;

//Nodes in Doubly linked lists.
struct Node  {
	int id; //id for each thread
	ucontext_t context;
	struct Node* next;
	struct Node* prev;
};

struct Node* head = NULL; // global variable - pointer to head node
struct Node* tail = NULL; // global variable - pointer to tail node
struct Node* currNode = NULL; // global variable - pointer to current node
int listsize = 0;
int totalnum = 0;

//Creates a new Node and returns pointer to it. 
struct Node* GetNewNode(ucontext_t ctx) {
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	listsize++;
	newNode->id = listsize - 1;
	newNode->context = ctx;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

//Inserts a Node at head of doubly linked list
void InsertAtHead(ucontext_t ctx) {
	struct Node* newNode = GetNewNode(ctx);
	if(head == NULL) {
		head = newNode;
		tail = newNode;
		return;
	}
	head->prev = newNode;
	newNode->next = head; 
	head = newNode;
}

//Inserts a Node at tail of Doubly linked list
void InsertAtTail(ucontext_t ctx) {
	//struct Node* temp = head;
	struct Node* newNode = GetNewNode(ctx);
	if(head == NULL) {
		head = newNode;
		tail = newNode;
		return;
	}
        tail->next = newNode;
	newNode->prev = tail;
	tail = newNode;
}

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg){
	// printf("start initializing a thread\n");
	// sleep(1);
	getcontext(&((*thread).ctx));
	(*thread).ctx.uc_stack.ss_sp = (*thread).st;
	(*thread).ctx.uc_stack.ss_size = sizeof((*thread).st);
	makecontext(&((*thread).ctx), &(*start_routine), 1, arg);
	totalnum++;
	// printf("finish initializing\n");

	return 0;
}

void mypthread_exit(void *retval){
	// printf("%d\n", numOfThread);
	setcontext(&coroutine_main);
}

int mypthread_yield(void){
	//printf("yielding thread %d\n", currNode->id);
	if(currNode != head) 
	{	
		// If this thread is not the head, then return to 
                // the previous thread in the list.
		currNode = currNode->prev;	
		swapcontext(&(currNode->next->context), &(currNode->context));
	} 
	else if(tail->id != totalnum - 1)
	{
		// If this thread is the head and the check thread has not yet been 
                // added, then return to the main thread. 
		swapcontext(&(currNode->context), &coroutine_main);
	}
	else
	{
		// If this thread is the head and the check thread has been 
                // added to the list, then return to the check thread.
		swapcontext(&(currNode->context), &(tail->context));
	}
	//printf("finish yielding\n");

	return 0;
}

int mypthread_join(mypthread_t thread, void **retval){
	// join method adds a new thread onto the tail of the linked list
	InsertAtTail(thread.ctx);
	currNode = tail;

	swapcontext(&coroutine_main, &(thread.ctx));	

	return 0;
}


