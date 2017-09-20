#ifndef _MINHEAP_H_ 
#define _MINHEAP_H_

#include "osm_types.h"

#define LCHILD(x) 2 * x + 1
#define RCHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

typedef struct node {
		int prio;
    OSM_Way* data;
} node ;

typedef struct minHeap {
    int size ;
    node *elem ;
} minHeap ;

minHeap initMinHeap(int size);
void swap(node *n1, node *n2);
void heapify(minHeap *hp, int i);
void buildMinHeap(minHeap *hp, int *arr, int size);
void insertNode(minHeap *hp, int prio, OSM_Way* way);
void deleteNode(minHeap *hp);
OSM_Way* getMaxNode(minHeap *hp, int i);
OSM_Way* getHead(minHeap *hp);
void deleteMinHeap(minHeap *hp);

#endif