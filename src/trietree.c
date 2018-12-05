# include "trietree.h"

void InitializeTree(Node *pn, int me)
{  
   pn=(Node*)malloc(sizeof(Node));
   pn->visited=false;
   pn->parent =NULL;
   pn->children =NULL;
   pn->item->score = -1;
   pn->item->alpha=10;
   pn->item->beta=0;
   pn->item->color=me;
   pn->item->piece=0;
   pn->item->command=0;
}

Node* CreatNode(int color,char piece,char command,int alpha,int beta)
{
    Node *pn = NULL;
    pn = (Node*)malloc(sizeof(Node));
    if(pnode == NULL)
    {
        return NULL;
    }
    pnode->visited = false;
    pnode->parent = NULL;
    pnode->children =NULL;
    pn->item->color = color;
    pn->item->piece = piece;
    pn->item->command = command;
    pn->item->alpha = alpha;
    pn->item->beta = beta;

   return pn;
}

void AddNode(Node* parent, Node* child)
{
    child->parent=parent;
    EnQueue(child,parent->children);
}

void EmptySubtree(Node* pn)
{
    while(pn->children)
    {
        QNode *child=NULL;
        DeQueue(child,pn->children);
        if(child->qitem->parent)
        {
            EmptySubtree(child->qitem);
            free(child->qitem);
        }
    }
}

Node* ChangeRootNode(Node* current, Node* new)
{
    current->parent=current;
    new->parent=NULL;
    EmptySubtree(current);
    return new;
}
//current_root = ChangeRootNode(current_root, new_root)