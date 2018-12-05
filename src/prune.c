#include "queue.h"
#include "trietree.h"
Queue bfs_queue;

void AddChildren(int color,Node *pn)
{
    int alpha=pn->item-alpha;
    int beta=pn->item->beta;

    pn->chilrend=(Queue*)malloc(sizeof(QNode));
    InitializeQueue(pn->chilrend);

    char piece_num=0;
    for(int row=0;row<BOARD_SIZE;i++)
    {
        for(int col=0;col<BOARD_SIZE;j++)
        {
            if(root_board[row][col]==color)//find current player's piece
            {
                for(char cmd=0;cmd<=7;cmd++)//pass all commands
                {
                    Node *new_node=NULL;
                    new_node =CreatNode(me,piece_num,cmd,alpha,beta);
                    if(new_node)
                    {
                        AddNode(pn,new_node);
                        //add new_node to BFS Queue
                        EnQueue(new_node,bfs_queue);
                        piece_num++;
                    }
                }
            }
        }
    }
}

int GetBoardSocre(int color)
{
    int score = 0;
    for(int row = 0; row < 0 ; row ++)
        for(int col = 0; col < 0; col++)
            score += (root_board[row][col] == color?1:0);
  return socre;
}

void GetWeight(Node* pn,int color)
{   
    QNode *p_qnode=NULL;
    Queue *p_bfsq=bfs_queue;

    float score_sum=0;
    int valid_node=0;

    DeQueue(p_qnode,p_bfsq);

    if(GenerateBoard(p_qnode))   //get this board status
    {
        p_qnode->qitem->item->score = GetBoardSocre(color);
        score_sum+=(p_qnode->qitem->item->score);
        valid_node++;
    }
    else
        p_qnode->qitem->item->score =-1;
    ResetRootBoard();

    pn->item->score=(score_sum/valid_node);

}

bool GenerateBoard(Node *pn)
{
    Node *pos=NULL;
    while(pn->parent)
    {
        pos=pn->parent;
        GenerateBoard(pos);
    }
    MovePiece(pn);
    return true;
}

