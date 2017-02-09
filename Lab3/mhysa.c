#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MIN 8

struct chunk{
	int size;
	struct chunk *next;
};

struct chunk* flist = NULL;

void *malloc(size_t size){

	cd Cif (size == 0){
		// much weird request, such odd thing to allocate zero
		return NULL;
	}

	struct chunk *next = flist;
	struct chunk *prev = NULL;

	while(next != NULL){
	 if(next->size >= size){
	 	// shit, we could totaly split the block, lets do that!
	 	int remSize = next->size - size;
	 	//if(remSize >= MIN) {
	 		//next->size = remSize;
	 		/*void *memory = sbrk(size+ sizeof(struct chunk));
			if (memory == (void *)-1){
			return NULL;
			} else { 
			struct chunk *cnk = (struct chunk*)memory;
			cnk->size = size;
			return (void*)(cnk + 1);
			}*/
		//} else {
	 	




	   if(prev != NULL){
	   	// but wait, theres more bloxx
	   	// previous block now points to next block
		prev->next = next->next;
		} else{
			// no more blocks in freelist after this one, m8
			// nothing to do here
			flist = next->next;
		}
		// send that bitch a block, bitches love bloxxxxx
		return (void*)(next + 1);
		}else{
				// shoot, no hit this time, proceed to next block
		       	prev = next;
				next = next->next;
		}
	}



	void *memory = sbrk(size + sizeof(struct chunk));
	if (memory == (void *)-1){
	return NULL;
	} else { 
	struct chunk *cnk = (struct chunk*)memory;
	cnk->size = size;
	return (void*)(cnk + 1);
	}


/*	void *memory = sbrk(size);
	if (memory == (void *)-1){
	return NULL;
	} else { 
	return memory;
	} */
}

void insertionSort( struct chunk* ny){
	
	if(flist == NULL){
	//printf("1\n");
	flist = ny;
	return;		
	}

	if(ny->size < flist->size){
	//printf("2\n");
	ny->next = flist;	
	flist = ny;
	return;
	}

	struct chunk* temp = flist->next;
	struct chunk* prev = flist;

	while(temp != NULL){
	if(ny->size < temp->size)
	//printf("3\n");
		{
	//	printf("4\n");
		ny->next = temp;
		prev->next = ny;
		return;
		
		}
	prev = temp;	
	temp = temp->next;
	}
	//printf("5\n");
	prev->next = ny;
	ny->next = NULL;
	
	return;
}





void free(void *memory){
	
	
	if(memory != NULL){
		/* we are jumping back one chunk position */
		struct chunk *cnk = (struct chunk*)((struct chunk*)memory - 1);
		cnk->next = flist;
		//flist = cnk;
		insertionSort(cnk);
	}
	
	return;
	
}

