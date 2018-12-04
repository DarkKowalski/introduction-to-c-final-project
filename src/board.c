void ResetRootBoard()
{
    memcpy(root_board,board,sizeof(char)*BOARD_SIZE*BOARD_SIZE);
}

bool RootPlace(int x, int y, OPTION option, int cur_flag) 
{
    // 移动之前的位置没有我方棋子
    if (root_board[x][y] != cur_flag) 
    {
        return false;
    }

  int new_x = x + DIR[option][0];
  int new_y = y + DIR[option][1];
  // 移动之后的位置超出边界, 或者不是空地
  if (!isInBound(new_x, new_y) || root_board[new_x][new_y] != EMPTY) 
    {
        return false;
    }

    root_board[x][y] = EMPTY;
    root_board[new_x][new_y] = cur_flag;

    int cur_other_flag = 3 - cur_flag;

    // 挑
    int intervention_dir[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
    for (int i = 0; i < 4; i++) 
    {
        int x1 = new_x + intervention_dir[i][0];
        int y1 = new_y + intervention_dir[i][1];
        int x2 = new_x - intervention_dir[i][0];
        int y2 = new_y - intervention_dir[i][1];
        if (isInBound(x1, y1) && isInBound(x2, y2) && root_board[x1][y1] == cur_other_flag && root_board[x2][y2] == cur_other_flag) 
        {
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
    if (isInBound(x1, y1) && isInBound(x2, y2) && root_board[x2][y2] == cur_flag && root_board[x1][y1] == cur_other_flag) 
    {
        root_board[x1][y1] = cur_flag;
    }
}

  return true;
}


void MovePiece(Node *pnode)
{
  int count = (int)((pnode->item->piece);
  char color = pnode->item->color;
  for(int row =0 ;row<BOARD_SIZE;row++)
    for(int col =0 ;col<BOARD_SIZE;col++)
      if(root_board[row][col]==color)
      {
        if(!(count>0))
            //target piece
            //move target piece and change table status
            RootPlace(col, row, command, pnode->item->color);
        else
          count--;
      }
}

int GetBoardSocre(char color)
{
    int score = 0;
    for(int row = 0; row < 0 ; row ++)
        for(int col = 0; col < 0; col++)
            score += (root_board[row][col] == color?1:0);
  return socre;
}