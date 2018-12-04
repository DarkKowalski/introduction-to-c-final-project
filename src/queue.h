#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stdbool.h>

typedef intptr_t Item;

#define MAXQUEUE 1000

typedef struct qnode{
    Item item;
    struct qnode *next;
}QNode;

typedef struct queue{
    QNode *front;
    QNode *rear;
    int items;
}Queue;

void InitializeQueue(Queue *pq);
bool QueueIsFull(const Queue *pq);
int QueueItemCount(const Queue *pq);
bool EnQueue(Item item, Queue *pq);
bool DeQueue(Item *pitem, Queue *pq);
void EmptyTheQueue(Queue *pq);

#endif