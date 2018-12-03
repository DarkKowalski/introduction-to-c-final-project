#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// board information
#define BOARD_SIZE 12
#define EMPTY 0
#define BLACK 1
#define WHITE 2

// bool
typedef int BOOL;
#define TRUE 1
#define FALSE 0

// option
typedef int OPTION;
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define UP_LEFT 4
#define UP_RIGHT 5
#define DOWN_LEFT 6
#define DOWN_RIGHT 7

#define MAX_BYTE 10000

#define START "START"
#define PLACE "PLACE"
#define TURN  "TURN"
#define END   "END"

struct Command
{
  int x;
  int y;
  OPTION option;
};

char buffer[MAX_BYTE] = {0};
char board[BOARD_SIZE][BOARD_SIZE] = {0};
int me_flag;
int other_flag;

const int DIR[8][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

void debug(const char *str) {
  printf("DEBUG %s\n", str);
  fflush(stdout);
}

void printBoard() {
  char visual_board[BOARD_SIZE][BOARD_SIZE] = {0};
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      if (board[i][j] == EMPTY) {
        visual_board[i][j] = '.';
      } else if (board[i][j] == BLACK) {
        visual_board[i][j] = 'O';
      } else if (board[i][j] == WHITE) {
        visual_board[i][j] = 'X';
      }
    }
    printf("%s\n", visual_board[i]);
  }
}

BOOL isInBound(int x, int y) {
  return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

/**
 * YOUR CODE BEGIN
 * 你的代码开始
 */

/*
 * Kowalski Dark <darkkowalski2012@gmail.com> 2018-12-02
 * 算法：广度搜索 + Alpha-Beta 剪枝
 * 思路：维护一个真实棋盘状态的备份作为 root_board
 * Node 中存储从上一 Node 到当前 Node 棋盘状态的改变方式：从上往下，从左往右，
 * 第一个自己的棋子标记为 0，存储在 curren_command ，该棋子移动方式存储在 current_command
 * getSocre 判断局面，当前 getSocre 所需的棋盘状态从 root_board 逐层推出
 * 
 * 搜索树实现，剪枝，咕咕中
 * getScore 待优化
 */

/**
 * You can define your own struct and variable here
 * 你可以在这里定义你自己的结构体和变量
 */

#define MAX_DEEPTH 4
#define ALL_PIECES 16
#define ALL_DIRECT 8

char root_board [BOARD_SIZE][BOARD_SIZE] = {0}; //copy of current "Board" which captured by "system"

void resetRootBoard()
{
  memcpy(root_board,board,sizeof(char)*BOARD_SIZE*BOARD_SIZE);
}

/*
  ADT:Search Tree 
*/
typedef struct item
{
  char current_command[2];
  char current_player;
  int score;
  char next_player_pieces;
}Item;

typedef struct node
{
  Item item;
  char deepth;
  char alpha;
  char beta;
  Node* parent;
  Node* children[item.next_player_pieces][ALL_DIRECT];
}Node;

/* 
  Functions of Search Tree
*/

void initializeTree(Node *ptree)
{
  *ptree = NULL;
}

Node* creatNode()
{
  Node *pnode = NULL;
  pnode = (Node*)malloc(sizeof(Node));
  if(pnode == NULL)
  {
    debug("creatNode: Malloc Error!");
    return NULL;
  }

  pnode->parent = NULL;
  pnode->children = NULL;

  return pnode;
}

Item* creatItem()
{
  Item *pitem=NULL;
  pitem = (Item*)malloc(sizeof(Item));
  if(pitem == NULL)
  {
    debug("creatItem: Malloc Error!");
    retunr NULL;
  }
  retun pitem;
}

void initializeItem(Item *pitem, char current_player,char which_piece,char what_command)
{
  pitem->curren_command[0] = which_piece;
  pitem->curren_command[1] = what_command;
  pitem->current_player= current_player;
}

void findParent(Node* parent, Node* child)
{
  child->parent = &parent;
}

BOOL addNode(Item item, Node *ptree)
{
  char which_piece=item.current_command[0];
  char what_command=iten.curren_command[1];
  if(ptree->children)
  {
    return FALSE;
  }
  else
  {
    ptree->children[which_piece][what_command]=creatNode();
    findParent(ptree, children[which_piece][what_command]);
    copyItemToNode(item, ptree->children[which_piece][what_command]);
  }
  return TRUE;
}

void emptyNode(Node *ptree)
{
  //TODO: release memory
}

void copyItemToNode(Item item, Node *pnode)
{
    pnode->item = item;
}

/* 
  That's all functions of Search Tree
*/

/* 
  fuctions to operate root board
*/
BOOL rootPlace(int x, int y, OPTION option, int cur_flag) {
  // 移动之前的位置没有我方棋子
  if (root_board[x][y] != cur_flag) {
    return FALSE;
  }

  int new_x = x + DIR[option][0];
  int new_y = y + DIR[option][1];
  // 移动之后的位置超出边界, 或者不是空地
  if (!isInBound(new_x, new_y) || root_board[new_x][new_y] != EMPTY) {
    return FALSE;
  }

  root_board[x][y] = EMPTY;
  root_board[new_x][new_y] = cur_flag;

  int cur_other_flag = 3 - cur_flag;

  // 挑
  int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
  for (int i = 0; i < 4; i++) {
    int x1 = new_x + intervention_dir[i][0];
    int y1 = new_y + intervention_dir[i][1];
    int x2 = new_x - intervention_dir[i][0];
    int y2 = new_y - intervention_dir[i][1];
    if (isInBound(x1, y1) && isInBound(x2, y2) && root_board[x1][y1] == cur_other_flag && root_board[x2][y2] == cur_other_flag) {
      root_board[x1][y1] = cur_flag;
      root_board[x2][y2] = cur_flag;
    }
  }

  // 夹
  int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
  for (int i = 0; i < 8; i++) {
    int x1 = new_x + custodian_dir[i][0];
    int y1 = new_y + custodian_dir[i][1];
    int x2 = new_x + custodian_dir[i][0] * 2;
    int y2 = new_y + custodian_dir[i][1] * 2;
    if (isInBound(x1, y1) && isInBound(x2, y2) && root_board[x2][y2] == cur_flag && root_board[x1][y1] == cur_other_flag) {
      root_board[x1][y1] = cur_flag;
    }
  }

  return TRUE;
}


void movePiece(Node *pnode)
{
  int count = (int)((pnode->item->current_command[0]);
  char player = pnode->item->current_player;
  for(int row =0 ;row<BOARD_SIZE;row++)
    for(int col =0 ;col<BOARD_SIZE;col++)
      if(root_board[row][col]==player)
      {
        if(!(count>0))
          //target piece
          //move target piece and change table status
          rootPlace(col, row, current_command[1], pnode->item->current_player);
        else
          count--;
      }
}

int getScore (int current_player)
{
  int score = 0;
  for(int row = 0; row < 0 ; row ++)
    for(int col = 0; col < 0; col++)
        score += (root_board[row][col] == current_player?1:0);
  return socre;
}

/**
 * 你可以在这里初始化你的AI
 */
void initAI(int me) {

}

struct Command findValidPos(const char board[BOARD_SIZE][BOARD_SIZE], int flag) {
  struct Command command = {0, 0, 0};
  for (int k = 0; k < 8; k++) {
    const int* delta = DIR[k];
    for (int x = 0; x < BOARD_SIZE; x++) {
      for (int y = 0; y < BOARD_SIZE; y++) {
        if (board[x][y] != flag) {
          continue;
        }
        int new_x = x + delta[0];
        int new_y = y + delta[1];
        if (isInBound(new_x, new_y) && board[new_x][new_y] == EMPTY) {
          command.x = x;
          command.y = y;
          command.option = k;
          return command;
        }
      }
    }
  }
  return command;
}

/**
 * 轮到你落子。
 * 棋盘上0表示空白，1表示黑棋，2表示白旗
 * me表示你所代表的棋子(1或2)
 * 你需要返回一个结构体Command，在x属性和y属性填上你想要移动的棋子的位置，option填上你想要移动的方向。
 */
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me) {
  /*
   * TODO：在这里写下你的AI。
   * 这里有一个示例AI，它只会寻找第一个可下的位置进行落子。
   */
 // struct Command preferedPos = findValidPos(board, me);

  

  return preferedPos;
}

/**
 * 你的代码结束
 */


BOOL place(int x, int y, OPTION option, int cur_flag) {
  // 移动之前的位置没有我方棋子
  if (board[x][y] != cur_flag) {
    return FALSE;
  }

  int new_x = x + DIR[option][0];
  int new_y = y + DIR[option][1];
  // 移动之后的位置超出边界, 或者不是空地
  if (!isInBound(new_x, new_y) || board[new_x][new_y] != EMPTY) {
    return FALSE;
  }

  board[x][y] = EMPTY;
  board[new_x][new_y] = cur_flag;

  int cur_other_flag = 3 - cur_flag;

  // 挑
  int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
  for (int i = 0; i < 4; i++) {
    int x1 = new_x + intervention_dir[i][0];
    int y1 = new_y + intervention_dir[i][1];
    int x2 = new_x - intervention_dir[i][0];
    int y2 = new_y - intervention_dir[i][1];
    if (isInBound(x1, y1) && isInBound(x2, y2) && board[x1][y1] == cur_other_flag && board[x2][y2] == cur_other_flag) {
      board[x1][y1] = cur_flag;
      board[x2][y2] = cur_flag;
    }
  }

  // 夹
  int custodian_dir[8][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
  for (int i = 0; i < 8; i++) {
    int x1 = new_x + custodian_dir[i][0];
    int y1 = new_y + custodian_dir[i][1];
    int x2 = new_x + custodian_dir[i][0] * 2;
    int y2 = new_y + custodian_dir[i][1] * 2;
    if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == cur_flag && board[x1][y1] == cur_other_flag) {
      board[x1][y1] = cur_flag;
    }
  }

  return TRUE;
}


//........
//........
//..XXXX..
//XXXX....
//....OOOO
//..OOOO..
//........
//........
void start(int flag) {
  memset(board, 0, sizeof(board));

  for (int i = 0; i < 3; i++) {
    board[2][2 + i] = WHITE;
    board[6][6 + i] = WHITE;
    board[5][3 + i] = BLACK;
    board[9][7 + i] = BLACK;
  }

  for (int i = 0; i < 2; i++) {
    board[8 + i][2] = WHITE;
    board[2 + i][9] = BLACK;
  }

  initAI(flag);
}

void turn() {
  // AI
  struct Command command = aiTurn((const char (*)[BOARD_SIZE])board, me_flag);
  place(command.x, command.y, command.option, me_flag);
  printf("%d %d %d\n", command.x, command.y, command.option);
  fflush(stdout);
}

void end(int x) {

}

void loop() {
//  freopen("../input", "r", stdin);
  while (TRUE)
  {
    memset(buffer, 0, sizeof(buffer));
    gets(buffer);

    if (strstr(buffer, START))
    {
      char tmp[MAX_BYTE] = {0};
      sscanf(buffer, "%s %d", tmp, &me_flag);
      other_flag = 3 - me_flag;
      start(me_flag);
      printf("OK\n");
      fflush(stdout);
    }
    else if (strstr(buffer, PLACE))
    {
      char tmp[MAX_BYTE] = {0};
      int x, y;
      OPTION option;
      sscanf(buffer, "%s %d %d %d", tmp, &x, &y, &option);
      place(x, y, option, other_flag);
    }
    else if (strstr(buffer, TURN))
    {
      turn();
    }
    else if (strstr(buffer, END))
    {
      char tmp[MAX_BYTE] = {0};
      int x;
      sscanf(buffer, "%s %d", tmp, &x);
      end(x);
    }
//    printBoard();
  }
}


int main(int argc, char *argv[]) {
  loop();
  return 0;
}
