#include "SPArrayList.h"

SPArrayList* spArrayListCreate(int maxSize){
	if(maxSize<=0)
		return NULL;

	SPArrayList* ret = (SPArrayList *)malloc(sizeof(SPArrayList));
    if(!ret){//memory allocation failed
		printf("Error: malloc has failed\n");
		return NULL;
	}
	ret->actualSize=0;
	ret->elements=(int *)malloc(maxSize*MOVE_SIZE*sizeof(int));

	if(!(ret->elements)){
        printf("Error: malloc has failed\n");
		return NULL; //memory allocation failed
	}
	ret->maxSize=maxSize;

	return ret;
}

SPArrayList* spArrayListCopy(SPArrayList* src){
	if(src==NULL)
		return NULL;

	SPArrayList* ret = spArrayListCreate(src->maxSize);
	if(!ret){//memory allocation failed
		return NULL;
	}
	ret->actualSize=src->actualSize;
	memcpy(ret->elements, src->elements, 4*MOVE_SIZE*(ret->actualSize)); //4 is int size - t bytes
	return ret;
}

void spArrayListDestroy(SPArrayList* src){
    if(src){
        free(src->elements);
        free(src);
    }
}

SP_ARRAY_LIST_MESSAGE spArrayListClear(SPArrayList* src){
	if(src==NULL)
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	src->actualSize=0;  // Since max capacity hasn't changed, we don't free allocated memory, but only update actual size.
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListAddAt(SPArrayList* src, int elem[], int index, char piece){
	if(src==NULL || index<0 || index>(src->actualSize))
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	if(src->actualSize==src->maxSize)
		return SP_ARRAY_LIST_FULL;

	for(int j=src->actualSize; j>index; j--) //Shifting every item in list one spot to the 'right'
		memcpy( (src->elements + j*MOVE_SIZE ) , (src->elements + (j-1)*MOVE_SIZE ), MOVE_SIZE*sizeof(int)); // - from the given index until end of list
	memcpy( (src->elements + index*MOVE_SIZE ) , elem, MOVE_SIZE*sizeof(int)); //Adding the new element (which is a 4-int path)
	*(src->elements + index*MOVE_SIZE + 4 ) = (int) piece;
	src->actualSize++;
	//*(ret->elements + index*MOVE_SIZE + indexInMove)
	return SP_ARRAY_LIST_SUCCESS;
}

 SP_ARRAY_LIST_MESSAGE spArrayListAddFirst(SPArrayList* src, int elem[], char piece){
	return spArrayListAddAt(src, elem, 0,piece);
 }

SP_ARRAY_LIST_MESSAGE spArrayListAddLast(SPArrayList* src, int elem[], char piece){
	return spArrayListAddAt(src, elem, src->actualSize, piece);
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveAt(SPArrayList* src, int index){
	if(src==NULL || index<0 || index>=(src->actualSize))
		return SP_ARRAY_LIST_INVALID_ARGUMENT;
	if(src->actualSize==0)
		return SP_ARRAY_LIST_EMPTY;
	for(int j=index; j<(src->actualSize)-1; j++) //Shifting every item in list one spot to the 'left'
		//(src->elements)[j]=(src->elements)[j+1];  // old version, where elements was dynamic path of ints, whereas the new is path of 4-int arrays
		memcpy( (src->elements + j*MOVE_SIZE ) , (src->elements + (j+1)*MOVE_SIZE ), MOVE_SIZE*sizeof(int)); // - from the given index until the end of the list
	src->actualSize--;
	return SP_ARRAY_LIST_SUCCESS;
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveFirst(SPArrayList* src){
	return spArrayListRemoveAt(src, 0);
}

SP_ARRAY_LIST_MESSAGE spArrayListRemoveLast(SPArrayList* src){
	return spArrayListRemoveAt(src, (src->actualSize)-1);
}

int *spArrayListGetAt(SPArrayList* src, int index){
	if(src==NULL || index<0 || index>=(src->actualSize))
		return NULL;
	return (src->elements + index*MOVE_SIZE );
}

int *spArrayListGetFirst(SPArrayList* src){
	return spArrayListGetAt(src, 0);
}

int* spArrayListGetLast(SPArrayList* src){
	return spArrayListGetAt(src, src->actualSize-1);
}

int spArrayListMaxCapacity(SPArrayList* src){
	if(src==NULL)
		return -1;
	return src->maxSize;
}

int spArrayListSize(SPArrayList* src){
	if(src==NULL)
		return -1;
	return src->actualSize;
}

bool spArrayListIsFull(SPArrayList* src){
	if(src==NULL || spArrayListSize(src)<spArrayListMaxCapacity(src))
		return false;
	return true;
}

bool spArrayListIsEmpty(SPArrayList* src){
	if(src==NULL || spArrayListSize(src)>0)
		return false;
	return true;
}
