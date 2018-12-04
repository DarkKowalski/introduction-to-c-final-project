#ifndef TREE_H_
#define TREE_H_
#include <stdlib.h>
#include "queue.h"

typedef struct item 
{
    char color;
    char piece;
    char command;
    int score;
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
 
void InitializeTree(Node *pn);
Node* CreatNode(char color,char piece,char command,int alpha,int beta);
void AddNode(Node* parent, Node* child) 
void EmptySubtree(Node *pn);
Node* ChangeRootNode(Node* current, Node* new);

#endif