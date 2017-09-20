#ifndef _OSM_ABR_H_
#define _OSM_ABR_H_

typedef struct ABR_Node{
	unsigned long int id;
	void* nd;
	struct ABR_Node *left;
	struct ABR_Node *right;
} ABR_Node;

void addNode(ABR_Node **tree, unsigned long int key, void* nd);
void* searchNode(ABR_Node *tree, unsigned long int  id);
void printTree(ABR_Node *tree);
void printReverseTree(ABR_Node *tree);
void clearTree(ABR_Node *tree);


#endif
