# include "trietree.h"

void InitializeTree(Node *pn)
{
   pn->parent =NULL;
   pn->children =NULL;
   pn->item->socre = -1;
   pn->item->alpha=10;
   pn->item->beta=0;
}

Node* CreatNode(char color,char piece,char command,int alpha,int beta)
{
    Node *pn = NULL;
    pn = (Node*)malloc(sizeof(Node));
    if(pnode == NULL)
    {
        return NULL;
    }
    pnode->parent = NULL;
    pnode->children = NULL;
    pn->item->color = color;
    pn->item->piece = piece;
    pn->item->command = command;
    pn->item->alpha = alpha;
    pn->item->beta = beta;

   return pn;
}

void AddNode(Node* parent, Node* child)
{
    child->parent=&parent;
    EnQueue(child,parent->children);
}

void EmptySubtree(Node* pn)
{
    while(pn->children)
    {
        Node *child=NULL;
        DeQueue(child,pn->children);
        if(child->parent)
        {
            EmptySubtree(child);
            free(child);
        }
    }
}

Node* ChangeRootNode(Node* current, Node* new)
{
    current->parent=&current;
    new->parent=NULL;
    EmptySubtree(current);
    return new;
}
//current_root = ChangeRootNode(current_root, new_root)