/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author: 
 * @date: 2015-11-20
 */

/* includes */
#include <stdio.h> 
#include <stdlib.h> 
#include "mem.h"

/* defines */

// The following code is the modified version of https://www.cs.cmu.edu/~guna/15-123S11/Lectures/Lecture11.pdf
// implementing a doubly linked list

typedef struct node {
  
 //void* data;
 struct node* next;
 struct node* prev;
 size_t size;
 int available; 
} node; 

/* global varaiables */

size_t totalSize = 0;
size_t nodeSize = sizeof(struct node);
struct node *linkedListHead;
/* Functions */

int memory_init(size_t size){
    
    if(size <= nodeSize ){
        //printf("invalid size !! \n");
        return -1; 
    }
    else{
        void *startAddress = malloc(size);
        if(startAddress == NULL){
            // printf("malloc failed!! \n");
             return -1;   
        }
        else{
            linkedListHead = (struct node *)startAddress;
            linkedListHead->next = linkedListHead->prev = NULL;
            totalSize = size;
            linkedListHead->size = totalSize - nodeSize;
            // printf("total size = %d and linkedListHead = %d", totalSize, linkedListHead-->size);
            linkedListHead->available = 1;
            return 0 ;
        }
    }
}
void both_dealloc(void *ptr){
    
    
    if(ptr == NULL){
        return;
    }
    // To be completed by students
    //
    struct node *blockToFree = (struct node *)(ptr - nodeSize);
    struct node *blockToFreeNxt =blockToFree->next ;
    struct node *blockToFreePrv = blockToFree->prev;
    
    blockToFree->available = 1;
    
    if((blockToFreePrv != NULL) && (blockToFreePrv->available) && (blockToFreeNxt != NULL) && (blockToFreeNxt->available)){
        blockToFreePrv->size = blockToFree->size + blockToFreePrv->size + blockToFreeNxt->size + nodeSize + nodeSize;
         
        blockToFreePrv->next = blockToFreeNxt->next;
        if(blockToFreeNxt->next !=NULL){
            blockToFreeNxt->next->prev = blockToFreePrv;
        }    
        
    }
    else if((blockToFreePrv != NULL) && (blockToFreePrv->available)){
        blockToFreePrv->size = blockToFree->size + blockToFreePrv->size +nodeSize;
        blockToFreePrv->next = blockToFreeNxt;
        if(blockToFreeNxt!=NULL){
           blockToFreeNxt->prev = blockToFreePrv;
        }    
        
    }
    else if((blockToFreeNxt != NULL) && (blockToFreeNxt->available)){
        blockToFree->size = blockToFree->size + blockToFreeNxt->size +nodeSize; 

        blockToFree->next = blockToFreeNxt->next;
       if(blockToFreeNxt->next !=NULL){
            blockToFreeNxt->next->prev = blockToFree;
        }    
            
    }
    
    return; 
}

int count_extfrag(size_t size){
    struct node *currentnode = linkedListHead; 
	int counter = 0;

	while(currentnode !=NULL) {
        
        if(currentnode->available && currentnode->size < size){
            counter++;
        }
		currentnode = currentnode->next;
	}

	return counter;
    
    
}


/* memory initializer */
int best_fit_memory_init(size_t size)
{
	// To be completed by students
	// You call malloc once here to obtain the memory to be managed.
    return memory_init(size);
}

int worst_fit_memory_init(size_t size)
{
	// To be completed by students
	// You call malloc once here to obtain the memory to be managed.
	return memory_init(size);
}

/* memory allocators */
void *best_fit_alloc(size_t size)
{
    size_t adjustedSize,bestfitRemainingSize ;
    struct node *currentnode = linkedListHead; 
    struct node *bestfitBlock = NULL;
    struct node *postSplitBlock = NULL;
    void * postSplitBlockPtr;
    void * returnVal;
    int bestfitFound = 0;
    
	// To be completed by students
     if(size <= 0 || size >= totalSize){
       // printf("invalid size !! \n");
        return NULL; 
    }
    else{
        // size has to be a multiply of 4
        // so we check if it it is a multiply of 4 and if it is not we make it 
        adjustedSize = size + ((size % 4 == 0) ? 0 : (4 - size % 4));
        while(currentnode != NULL){
            if((currentnode->available) && (currentnode->size >= adjustedSize ) ){
                // if we havent found a bestfit block before, this is it! else compare if this is better fit
                if(!bestfitFound){
                    bestfitBlock = currentnode;
                    bestfitFound = 1;
                }
                else{
                    //then we have to compare if there is a better fit
                    //printf("current size = %d, bestfit size = %d\n",currentnode->size,bestfitBlock->size)
                    if(currentnode->size < bestfitBlock->size){
                        //better fit found
                        bestfitBlock = currentnode;
                    }
                }  
            }
            currentnode = currentnode->next;
        }
        if(!bestfitFound){
           // printf("could not fine a free block in best fit ! \n");
            return NULL;   
        }
        else{
            bestfitRemainingSize = bestfitBlock->size - adjustedSize;
            if(bestfitRemainingSize > nodeSize){
                //remaining size if we split is big enough to hold a node to represent that block yay
                postSplitBlockPtr = (void *)(bestfitBlock) + nodeSize +adjustedSize;
                postSplitBlock = (struct node *)postSplitBlockPtr;
                
                postSplitBlock->next = bestfitBlock->next;
                postSplitBlock->prev = bestfitBlock;
                if(bestfitBlock->next!= NULL){
                    bestfitBlock->next->prev =postSplitBlock;  
                }
                bestfitBlock->next = postSplitBlock;
                postSplitBlock->size = bestfitRemainingSize -nodeSize;
                postSplitBlock->available = 1;
            }
            
            bestfitBlock->available = 0;
            bestfitBlock->size = adjustedSize;
            
             returnVal = (void*) bestfitBlock + nodeSize;
             return returnVal;
            
        }
        
    }
}


void *worst_fit_alloc(size_t size)
{
    size_t adjustedSize,worstfitRemainingSize ;
    struct node *currentnode = linkedListHead; 
    struct node *worstfitBlock = NULL;
    struct node *postSplitBlock = NULL;
    void * postSplitBlockPtr;
    void * returnVal;
    int worstfitFound = 0;
    
	// To be completed by students
     if(size <= 0 || size >= totalSize){
       // printf("invalid size !! \n");
        return NULL; 
    }
    else{
        // size has to be a multiply of 4
        // so we check if it it is a multiply of 4 and if it is not we make it 
        adjustedSize = size + ((size % 4 == 0) ? 0 : (4 - size % 4));
        while(currentnode != NULL){
            if((currentnode->available) && (currentnode->size >= adjustedSize ) ){
                // if we havent found a worstfit block before, this is it! else compare if this is better fit
                if(!worstfitFound){
                    worstfitBlock = currentnode;
                    worstfitFound = 1;
                }
                else{
                    //then we have to compare if there is a better fit
                    //printf("current size = %d, worstfit size = %d\n",currentnode->size,worstfitBlock->size)
                    if(currentnode->size > worstfitBlock->size){
                        //better fit found
                        worstfitBlock = currentnode;
                    }
                }  
            }
            currentnode = currentnode->next;
        }
        if(!worstfitFound){
            //printf("could not fine a free block in best fit ! \n");
            return NULL;   
        }
        else{
            worstfitRemainingSize = worstfitBlock->size - adjustedSize;
            if(worstfitRemainingSize > nodeSize){
                //remaining size if we split is big enough to hold a node to represent that block yay
                postSplitBlockPtr = (void *)(worstfitBlock) + nodeSize +adjustedSize;
                postSplitBlock = (struct node *)postSplitBlockPtr;
                
                postSplitBlock->next = worstfitBlock->next;
                postSplitBlock->prev = worstfitBlock;
                if(worstfitBlock->next!= NULL){
                    worstfitBlock->next->prev =postSplitBlock;  
                }
                worstfitBlock->next = postSplitBlock;
                postSplitBlock->size = worstfitRemainingSize -nodeSize;
                postSplitBlock->available = 1;
                worstfitRemainingSize = 0;
            }
            
            worstfitBlock->available = 0;
            worstfitBlock->size = adjustedSize + worstfitRemainingSize;
            
             returnVal = (void*) worstfitBlock + nodeSize;
             return returnVal;
            
        }
        
    }
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{
    // To be completed by students
    both_dealloc(ptr);
    return;
}

void worst_fit_dealloc(void *ptr) 
{
	// To be completed by students
    both_dealloc(ptr);
    return;
}
/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size)
{
	// To be completed by students
	return count_extfrag(size);
}

int worst_fit_count_extfrag(size_t size)
{
	// To be completed by students
	return count_extfrag(size);
}