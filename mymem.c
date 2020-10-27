#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = Worst;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	// traverse the whole list to free the nodes.
	if(head!=NULL){
	    struct memoryList *trav = head;
	    struct memoryList *nextnode;
	    while (trav!=NULL){
	        nextnode = trav->next;
	        free(trav);
	        trav = nextnode;
	    }

	}



	myMemory = malloc(sz);

	head = (struct memoryList*) malloc(sizeof(struct memoryList)*1);
	head->size=sz;
	head->alloc=0;
	head->next=NULL;
	head->last=NULL;
	head->ptr=myMemory;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);
    struct memoryList *trav;
    struct memoryList *worstNode;
    if(mem_largest_free()<requested){
        return NULL;
    }
	switch (myStrategy)
	  {
	  case NotSet:
	            return NULL;
	  case First:
	            return NULL;
	  case Best:
	            return NULL;

          case Worst:
              trav = head;
              int biggestSize = 0;
              while (trav!=NULL) {
                  if (trav->size >= requested && trav->size > biggestSize && trav->alloc == 0) {
                      worstNode = trav;
                      biggestSize = trav->size;
                  }
                  // to next node as in for loop i++;
                  trav = trav->next;
              }
              if(worstNode==NULL) return NULL;

              // No need to allocate a new memmory block if the node size equals requested.
              if(worstNode->size==requested && worstNode->alloc == 0){
                  worstNode->alloc = 1;
                  return worstNode->ptr;
              }

              if(worstNode->size>requested && worstNode!=0){
                   struct memoryList *newMemoryBlock = (struct memoryList*) malloc(sizeof(struct memoryList)*1);
                   newMemoryBlock->size=requested;
                   worstNode->size=worstNode->size-requested;
                   newMemoryBlock->alloc=1;
                   if(worstNode->last!=NULL){
                       newMemoryBlock->last =worstNode->last;
                       worstNode->last->next = newMemoryBlock;
                   } else{
                       newMemoryBlock->last=NULL;
                   }
                   if(worstNode->ptr == head->ptr) head=newMemoryBlock;
                   newMemoryBlock->next = worstNode;
                   newMemoryBlock->ptr = worstNode->ptr;

                   worstNode->ptr = worstNode->ptr+requested;
                   worstNode->last = newMemoryBlock;

                  return newMemoryBlock->ptr;
              }
	            return NULL;
	  case Next:
	            return NULL;
	  }
	return NULL;
}




/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
    struct memoryList *trav = head;
    while (trav!=NULL){
        if(trav->ptr==block){
            trav->alloc=0;

            //check the last and merge into the current
            //last into this
            if(trav->last!=NULL && trav->last->alloc==0){
                trav->size += trav->last->size;
                if(trav->last->last!=NULL){
                    struct memoryList *lastToFree = trav->last;
                    trav->last->last->next = trav;
                    trav->last = trav->last->last;
                    free(lastToFree);
                } else{
                    trav->last=NULL;
                    free(trav->last);
                    head = trav;
                }
            }

            //now merge next node into current
            if(trav->next!=NULL && trav->next->alloc==0){
                trav->size+= trav->next->size;
                if(trav->next->next != NULL){
                    struct memoryList *nextToFree = trav->next;
                    trav->next->next->last = trav;
                    trav->next = trav->next->next;
                    free(nextToFree);
                }
                else {
                    free(trav->next);
                    trav->next = NULL;
                }
            }
            return;
        }
        trav=trav->next;
    }

    return;

}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
    struct memoryList *trav = head;
    int memholes = 0;
    while (trav!=NULL){
        if(trav->alloc==0 && trav->size>0)
            memholes++;
        trav=trav->next;
    }
    return memholes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
    struct memoryList *trav = head;
    int memAllocated = 0;
    while (trav!=NULL){
        if(trav->alloc==1) memAllocated+=trav->size;
        trav=trav->next;
    }
    return memAllocated;
}

/* Number of non-allocated bytes */
int mem_free()
{
   struct memoryList *trav = head;
    int memFree = 0;
    while (trav!=NULL){
        if(trav->alloc==0) memFree+=trav->size;
        trav=trav->next;
    }
    return memFree;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
    struct memoryList *trav = head;
    int memLargestFree = 0;
    while (trav!=NULL){
        if(trav->alloc==0){
            if(trav->size>memLargestFree){
                memLargestFree=trav->size;
            }
        }
        trav=trav->next;
    }
    return memLargestFree;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
    struct memoryList *trav = head;
    int memSmallFree = 0;
    while (trav!=NULL){
        if(trav->alloc==0 && trav->size<size){
            memSmallFree++;
        }
        trav=trav->next;
    }
    return memSmallFree;
}

char mem_is_alloc(void *ptr)
{
    struct memoryList *trav=head;
    while (trav!=NULL){
        if(trav->alloc==1 && trav->ptr==ptr)
            return 1;
        trav=trav->next;
    }
    return 0;
}

/*
 * Feel free to use these functions, but do not modify them.
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy.
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/*
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
    struct memoryList *temp = head;
    int i =1;
    char *str ="Not Allocated";
    while (temp!=NULL){
        printf("Block: %d, ",i);
        printf("size: %d, ",temp->size);
        printf("allocated: %d, \n",temp->alloc);
        i++;
        temp=temp->next;
    }
}

/* Use this function to track memory allocation performance.
 * This function does not depend on your implementation,
 * but on the functions you wrote above.
 */
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;


	/* A simple example.
	   Each algorithm should produce a different layout. */

	initmem(strat,500);

	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);

	print_memory();
	print_memory_status();

}
