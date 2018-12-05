#ifndef TREE_H_
#define TREE_H_
#include <stdlib.h>
#include "queue.h"

typedef struct item 
{
    int color;
    char piece;
    char command;
    float score;
    int alpha;
    int beta;
}Item;

typedef struct node
{
    Item item;
    Node* parent;
    Queue* chilrend;
}Node;

Node* current_root=NULL;
Node* current_operate=NULL;
 
void InitializeTree(Node *pn,int me);
Node* CreatNode(int color,char piece,char command,int alpha,int beta);
void AddNode(Node* parent, Node* child) 
void EmptySubtree(Node *pn);
Node* ChangeRootNode(Node* current, Node* new);

#endif