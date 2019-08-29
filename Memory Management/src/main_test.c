/**
  * ECE254 Linux Dynamic Memory Management Lab
  * @file: main_test.c
  * @brief: The main file to write tests of memory allocation algorithms
  */ 

/* includes */
/* system provided header files. You may add more */
#include <stdio.h>

/* non-system provided header files. 
   Do not include more user-defined header files here
 */
#include "mem.h"



int main(int argc, char *argv[])
{
    void **arrPointer = malloc(sizeof(void *) * 1000);
    void **bestArr = malloc(sizeof(void *) * 1000);
    void **worstArr = malloc(sizeof(void *) * 1000);
    int number = 2;
    int fragNum = 0;
    int i;
    int randNumb;
    int mega = 1024*1024;
    
	void *p,*p1,*p2, *q,*q1,*q2;
	
    
    i=0;
    for (; i < 1000; i++){
		arrPointer[i] = NULL;
    }
    
    //printf("the value of node size is %d\n",nodeSize);
    printf("case 1, allocating and dealocating the same values\n");
    printf("best_fit allocation started\n");
	best_fit_memory_init(1024);	// initizae 1KB, best fit
        i=0;
    	for (; i < 5; i++) {
		arrPointer[i] = best_fit_alloc(number);
		printf("best_fit allocated p=%p for the size of %d\n", arrPointer[i], number);
		number = number *2;
	}
	printf("best_fit allocation completed\n");
    printf("\n");
    printf("best_fit deallocation started\n");
	for (;i >= 0; i--) {
        if ( arrPointer[i] == NULL ) {
		break;	
	}
		best_fit_dealloc(arrPointer[i]);
        arrPointer[i] = NULL;
	}
    printf("best_fit deallocation completed\n");
    fragNum =  best_fit_count_extfrag(4);
    printf("frag count for 4  and 5 allocation and 5 deallocations is%d\n", fragNum);
    printf("\n");
    ////////////////////////
    printf("worst_fit allocation started\n");
	worst_fit_memory_init(1024);	// initizae 1KB, best fit
        i=0;
        number =2;
    	for (; i < 5; i++) {
		arrPointer[i] = worst_fit_alloc(number);
		printf("worst_fit allocated p=%p for the size of %d\n", arrPointer[i], number);
		number = number *2;
	}
	printf("worst_fit allocation completed\n");
    printf("\n");
    printf("worst_fit deallocation started\n");
	for (;i >= 0; i--) {
        if ( arrPointer[i] == NULL ) {
		break;	
	}
		worst_fit_dealloc(arrPointer[i]);
        arrPointer[i] = NULL;
	}
    printf("worst_fit deallocation completed\n");
    fragNum =  worst_fit_count_extfrag(4);
    printf("frag count for 4  and 5 allocation and 5 deallocations is%d\n", fragNum);
    printf("\n");
    
    ///////////////
    
       srand((unsigned int)time(0));
       i=0;
    for (; i < 1000; i++){
		arrPointer[i] = NULL;
    }
    printf("case 2, allocating random numbers  and dealocating some other randome numbers\n");
    printf("best_fit allocation started\n");
	best_fit_memory_init(mega);	// initizae 1KB, best fit
        i=0;
    	for (; i < 500; i++) {
        randNumb = rand()%100;
        if(randNumb < 2)
        {
            randNumb = 2;  
        }
		arrPointer[i] = best_fit_alloc(randNumb);
        //if ( arrPointer[i] == NULL ) {
		//break;	
	//}
		printf("best_fit allocated p=%p for the size of %d\n", arrPointer[i], randNumb);
	}
	printf("best_fit allocation completed\n");
    printf("\n");
    printf("best_fit deallocation started\n");
    i=250;
	for (;i >= 0; i--) {
        if ( arrPointer[i] == NULL ) {
		break;	
	}
        randNumb = rand()%i;
		best_fit_dealloc(arrPointer[randNumb]);
        arrPointer[randNumb] = NULL;
	}
    printf("best_fit deallocation completed\n");
    fragNum =  best_fit_count_extfrag(8);
    printf("frag count for 8  is %d\n", fragNum);
    fragNum =  best_fit_count_extfrag(50);
    printf("frag count for 50  %d\n", fragNum);
    fragNum =  best_fit_count_extfrag(200);
    printf("frag count for 200 is%d\n", fragNum);
    fragNum =  best_fit_count_extfrag(500);
    printf("frag count for 500 is%d\n", fragNum);
    printf("\n");
    
    //////////////////////
     i=0;
    for (; i < 1000; i++){
		arrPointer[i] = NULL;
    }
    printf("worst_fit allocation started\n");
	worst_fit_memory_init(mega);	// initizae 1KB, worst fit
        i=0;
    	for (; i < 500; i++) {
        randNumb = rand()%100;
        if(randNumb < 2)
        {
            randNumb = 2;  
        }
		arrPointer[i] = worst_fit_alloc(randNumb);
       // if ( arrPointer[i] == NULL ) {
		//break;	
	//}
		printf("worst_fit allocated p=%p for the size of %d\n", arrPointer[i], randNumb);
	}
	printf("worst_fit allocation completed\n");
    printf("\n");
    printf("worst_fit deallocation started\n");
    i=250;
	for (;i >= 0; i--) {
        if ( arrPointer[i] == NULL ) {
		break;	
	}
        randNumb = rand()%i;
		worst_fit_dealloc(arrPointer[randNumb]);
        arrPointer[randNumb] = NULL;
	}
    printf("worst_fit deallocation completed\n");
    fragNum =  worst_fit_count_extfrag(8);
    printf("frag count for 8  is %d\n", fragNum);
    fragNum =  worst_fit_count_extfrag(50);
    printf("frag count for 50  %d\n", fragNum);
    fragNum =  worst_fit_count_extfrag(200);
    printf("frag count for 200 is%d\n", fragNum);
    fragNum =  worst_fit_count_extfrag(500);
    printf("frag count for 500 is%d\n", fragNum);
    printf("\n");
    
    ////////////////
    i=0;
    for (; i < 1000; i++){
		bestArr[i] = NULL;
        worstArr[i] = NULL;
    }
    printf("case 3, allocating random numbers  and dealocating some other randome numbers for both best and worst\n");
    worst_fit_memory_init(mega);	// initizae 1KB, worst fit
    best_fit_memory_init(mega);
        i=0;
    	for (; i < 500; i++) {
        randNumb = rand()%100;
        if(randNumb < 2)
        {
            randNumb = 2;  
        }
		worstArr[i] = worst_fit_alloc(randNumb);
        bestArr[i] = best_fit_alloc(randNumb);
       // if ( bestArr[i] == NULL || worstArr[i] == NULL ) {
		//break;	
	//}
		printf("worst_fit allocated p=%p for the size of %d\n", worstArr[i], randNumb);
        printf("best_fit allocated p=%p for the size of %d\n", bestArr[i], randNumb);
	}
	printf("allocation completed\n");
    printf("\n");
    printf("deallocation started\n");
    i=250;
	for (;i >= 0; i--) {
        if ( bestArr[i] == NULL || worstArr[i] == NULL  ) {
		break;	
	}
        randNumb = rand()%i;
		worst_fit_dealloc(worstArr[randNumb]);
        worstArr[randNumb] = NULL;
        best_fit_dealloc(bestArr[randNumb]);
        bestArr[randNumb] = NULL;
	}
    printf("worst_fit deallocation completed\n");
    fragNum =  best_fit_count_extfrag(50);
    printf("best frag count for 50 is %d\n", fragNum);
    fragNum =  worst_fit_count_extfrag(50);
    printf("worst frag count for 50 is %d\n", fragNum);
    fragNum =  best_fit_count_extfrag(200);
    printf("best frag count for 200 is %d\n", fragNum);
    fragNum =  worst_fit_count_extfrag(200);
    printf("worst frag count for 200 is %d\n", fragNum);
    fragNum =  best_fit_count_extfrag(500);
    printf("best frag count for 500 is%d\n", fragNum);
    fragNum =  worst_fit_count_extfrag(500);
    printf("worst frag count for 500 is%d\n", fragNum);
    printf("\n");
    

	return 0;
}