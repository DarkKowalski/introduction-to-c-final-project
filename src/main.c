#pragma GCC optimize ("O3")
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

char buffer[MAX_BYTE] = { 0 };
char board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int me_flag;
int other_flag;

const int DIR[8][2] = { { -1, 0 },{ 1, 0 },{ 0, -1 },{ 0, 1 },{ -1, -1 },{ -1, 1 },{ 1, -1 },{ 1, 1 } };

void debug(const char *str) 
{
	printf("DEBUG %s\n", str);
	fflush(stdout);
}

void printBoard() 
{
	char visual_board[BOARD_SIZE][BOARD_SIZE] = { 0 };

	for (int i = 0; i < BOARD_SIZE; i++) 
	{
		for (int j = 0; j < BOARD_SIZE; j++) 
		{
			if (board[i][j] == EMPTY) 
			{
				visual_board[i][j] = '.';
			}
			else if (board[i][j] == BLACK) 
			{
				visual_board[i][j] = 'O';
			}
			else if (board[i][j] == WHITE) 
			{
				visual_board[i][j] = 'X';
			}
		}
	}

	printf("  0 1 2 3 4 5 6 7 8 9 0 1\n");
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		printf("%d ", i % 10);
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			printf("%c ", visual_board[i][j]);
		}
		printf("\n");
	}
}

static inline BOOL isInBound(int x, int y) 
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

/**
* YOUR CODE BEGIN
*/

#include <stdbool.h>
#include <math.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define ALLCMD 8				// total number of commands
#define ALLDIR 8				// total number of directions

#undef INFINITY
#define INFINITY 1145141919

#define BOOTALPHA -INFINITY		// initial alpha for alpha-beta pruning
#define BOOTBETA INFINITY		// initial beta for alpha-beta pruning

#define ENEMY_SURROUND 2        // number of enemy pieces which is greater or equal to this value will trigger FUNCTION: enemyPiecesSurround

#define PIECE_WEIGHT 1000
#define BAD_FORM_WEIGHT 500      // increase this value to enhance Defensiveness
#define GOOD_FORM_WEIGHT 100     // increase this value to enhance Aggression
#define DISTANCE_WEIGHT 1        // increase this value to enhance Defensiveness
#define SURROUND_WEIGHT 1        // increase this value to enhance Aggression

#define MAXDEPTH 4				// maximum depth for DFS
#define FIRST_STEP_LIMIT 20		// initial width limit for DFS

typedef struct ChildBoard
{
	int x;
	int y;
	int cmd;
	int history_score;
}Child;

typedef struct ShallowBoard
{
	int x;
	int y;
	int cmd;
	int score;
}Sboard;

static int history_table[BOARD_SIZE][BOARD_SIZE][ALLCMD] = { 0 };

int getDistance(const char node_board[BOARD_SIZE][BOARD_SIZE],int x, int y) // measure distance to the nearest enemy piece
{
	int distance = 1; // Chebyshev distance: https://en.wikipedia.org/wiki/Chebyshev_distance
	int scan_x, scan_y;

	for (distance = 1; distance < BOARD_SIZE; distance++) 
	{
		scan_x = x - distance;
		scan_y = y - distance;

		int scan_limit = 2 * distance;

		if (isInBound(scan_x, scan_y) && node_board[scan_x][scan_y] == other_flag)
		{
			return distance;
		}

		for (int i = 0; i <= scan_limit; i++)
		{
			scan_x ++;

			if (!isInBound(scan_x, scan_y))
			{
				continue;
			}

			if (isInBound(scan_x, scan_y) && node_board[scan_x][scan_y] == other_flag)
			{
				return distance;
			}
		}

		for (int i = 0; i <= scan_limit; i++)
		{
			scan_y ++;
			if (!isInBound(scan_x, scan_y))
			{
				continue;
			}
			if (isInBound(scan_x, scan_y) && node_board [scan_x][scan_y] == other_flag)
			{
				return distance;
			}
		}

		for (int i = 0; i <= scan_limit; i++)
		{
			scan_x --;
			if (!isInBound(scan_x, scan_y))
			{
				continue;
			}
			if (isInBound(scan_x, scan_y) && node_board[scan_x][scan_y] == other_flag)
			{
				return distance;
			}
		}

		for (int i = 0; i <= scan_limit; i++)
		{
			scan_y --;
			if (!isInBound(scan_x, scan_y))
			{
				continue;
			}
			if (isInBound(scan_x, scan_y) && node_board[scan_x][scan_y] == other_flag)
			{
				return distance;
			}
		}
	}

	return 0;
}

bool enemyPiecesSurround (const char node_board[BOARD_SIZE][BOARD_SIZE], int x, int y)
{
	int enemy_pieces = 0;

	for (int scan_dir = 0; scan_dir < ALLDIR; scan_dir++) 
	{
		int scan_x = x + DIR[scan_dir][0];
		int scan_y = y + DIR[scan_dir][1];
		if (node_board[scan_x][scan_y] == other_flag)
		{
			enemy_pieces++; 
			if (enemy_pieces == ENEMY_SURROUND)
			{
				return true;
			}
		}
	}
	return false;
}

int getScore(char node_board[BOARD_SIZE][BOARD_SIZE])
{
	int my_pieces_num = 0;
	int distance_to_enemy = 0;
	int enemy_nearby_cases = 0;
	int good_form_case = 0;
	int bad_form_case = 0;

	for (int scan_x = 0; scan_x < BOARD_SIZE; scan_x++)
	{
		for (int scan_y = 0; scan_y < BOARD_SIZE; scan_y++)
		{
			if (node_board[scan_x][scan_y] == EMPTY)
				continue;
			else if(node_board[scan_x][scan_y] == me_flag)
			{
				my_pieces_num++; 
				distance_to_enemy += getDistance(node_board, scan_x, scan_y);

				if (enemyPiecesSurround(node_board, scan_x, scan_y))
					enemy_nearby_cases++;
			}

			for (int scan_dir = 0; scan_dir < 8; scan_dir++)
			{
				
				int test_clamp_x = scan_x - DIR[scan_dir][0];
				int test_clamp_y = scan_y - DIR[scan_dir][1];

				int test_pole_x = scan_x + 2 * DIR[scan_dir][0];
				int test_pole_y = scan_y + 2 * DIR[scan_dir][1];

				int test_empty_x = scan_x + DIR[scan_dir][0];
				int test_empty_y = scan_y + DIR[scan_dir][1];

				if (node_board[scan_x][scan_y] == me_flag)
				{
					if ((node_board[test_empty_x][test_empty_y] == EMPTY && node_board[test_clamp_x][test_clamp_y] == other_flag) || 
						(node_board[test_empty_x][test_empty_y] == EMPTY && node_board[test_pole_x][test_pole_y] == me_flag))
					{
						for (int scan_empty_dir = 0; scan_empty_dir < ALLDIR; scan_empty_dir++)
						{
							int test_form_x = test_empty_x + DIR[scan_empty_dir][0];
							int test_form_y = test_empty_y + DIR[scan_empty_dir][1];
							if (node_board[test_form_x][test_form_y] == other_flag)
							{
								bad_form_case++; // lose my pieces in next turn
								break;
							}
						}
					}
				}
				if (node_board[scan_x][scan_y] == other_flag)
				{
					if ((node_board[test_empty_x][test_empty_y] == EMPTY && node_board[test_clamp_x][test_clamp_y] == me_flag)
						|| (node_board[test_empty_x][test_empty_y] == EMPTY && node_board[test_pole_x][test_pole_y] == other_flag))
					{
						for (int scan_dir_2 = 0; scan_dir_2 < ALLDIR; scan_dir_2++)
						{
							int test_form_x = test_empty_x + DIR[scan_dir_2][0];
							int test_form_y = test_empty_y + DIR[scan_dir_2][1];
							if (node_board[test_form_x][test_form_y] == me_flag)
							{
								good_form_case++; // get new pieces in next turn
								break;
							}
						}
					}
				}
			}
		}
	}

	int final_score = 0;

	final_score += my_pieces_num * PIECE_WEIGHT;
	final_score += good_form_case * GOOD_FORM_WEIGHT;
	final_score += enemy_nearby_cases * SURROUND_WEIGHT;

	final_score -= bad_form_case * BAD_FORM_WEIGHT;
	final_score -= distance_to_enemy * DISTANCE_WEIGHT;

	return final_score;
}

bool placeInNodeBoard (char(*node_board)[BOARD_SIZE], int x, int y, int option, int cur_flag)
{
	if (node_board[x][y] != cur_flag)
	{
		return false;
	}

	int new_x = x + DIR[option][0];
	int new_y = y + DIR[option][1];

	if (!isInBound(new_x, new_y) || node_board[new_x][new_y] != EMPTY)
	{
		return false;
	}

	node_board[x][y] = EMPTY;
	node_board[new_x][new_y] = cur_flag;

	int cur_other_flag = 3 - cur_flag;


	int intervention_dir[4][2] = { { 1, 0 },{ 0, 1 },{ 1, 1 },{ 1, -1 } };
	for (int i = 0; i < 4; i++)
	{
		int x1 = new_x + intervention_dir[i][0];
		int y1 = new_y + intervention_dir[i][1];
		int x2 = new_x - intervention_dir[i][0];
		int y2 = new_y - intervention_dir[i][1];
		if (isInBound(x1, y1) && isInBound(x2, y2) && node_board[x1][y1] == cur_other_flag && node_board[x2][y2] == cur_other_flag)
		{
			node_board[x1][y1] = cur_flag;
			node_board[x2][y2] = cur_flag;
		}
	}

	int custodian_dir[8][2] = { { 1, 0 },{ -1, 0 },{ 0, 1 },{ 0, -1 },{ 1, 1 },{ 1, -1 },{ -1, 1 },{ -1, -1 } };
	for (int i = 0; i < 8; i++) {
		int x1 = new_x + custodian_dir[i][0];
		int y1 = new_y + custodian_dir[i][1];
		int x2 = new_x + custodian_dir[i][0] * 2;
		int y2 = new_y + custodian_dir[i][1] * 2;
		if (isInBound(x1, y1) && isInBound(x2, y2) && node_board[x2][y2] == cur_flag && node_board[x1][y1] == cur_other_flag)
		{
			node_board[x1][y1] = cur_flag;
		}
	}

	return true;
}

int highHistoryScore(const void *a, const void *b)
{
	return ((Child*)b)->history_score - ((Child*)a)->history_score;
}

int highShallowScore(const void *a, const void *b)
{
	return ((Sboard*)b)->score - ((Sboard*)a)->score ;
}

int alphaBeta(const char node_board[][BOARD_SIZE], int depth, int prune_alpha, int prune_beta, int player)
{
	char board_operate[BOARD_SIZE][BOARD_SIZE] = { 0 };
	memcpy(board_operate, node_board, sizeof(char)*BOARD_SIZE*BOARD_SIZE);

	Child child_board[16 * ALLCMD] = { 0 };
	int child_offset = 0;

	int score = 0;
	if (depth == 0)
	{
		score = getScore(board_operate);
		return score;
	}

	if (player == me_flag)
	{
		score = -INFINITY;

		// generate all possible moves and save them in local array child_board
		for (int x = 0; x <BOARD_SIZE; x++)
		{
			for (int y = 0; y<BOARD_SIZE; y++)
			{
				if (board_operate[x][y] == me_flag)
				{
					for (int cmd = 0; cmd<ALLCMD; cmd++)
					{
						memcpy(board_operate, node_board, sizeof(char)*BOARD_SIZE*BOARD_SIZE);
						bool move_success = false;
						move_success = placeInNodeBoard(board_operate, x, y, cmd, me_flag);

						if (move_success)
						{
							// saved status
							(child_board[child_offset]).x = x;
							(child_board[child_offset]).y = y;
							(child_board[child_offset]).cmd = cmd;
							(child_board[child_offset]).history_score = history_table[x][y][cmd];
							child_offset++;
						}
					}
				}
			}
		}

		// sort nodes by history_score, from high to low
		qsort(child_board, 16 * ALLCMD, sizeof(Child), highHistoryScore);

		for (int i = 0; i<child_offset; i++)
		{
			memcpy(board_operate, node_board, sizeof(char)*BOARD_SIZE*BOARD_SIZE);
			placeInNodeBoard(board_operate, child_board->x, child_board->y, child_board->cmd, me_flag);

			int new_score = (alphaBeta(board_operate, depth - 1, prune_alpha, prune_beta, other_flag));
			score = MAX(score, new_score);
			prune_alpha = MAX(prune_alpha, score);

			if (prune_beta <= prune_alpha)
			{
				// trigger pruning and increase history_score
				history_table[child_board[i].x][child_board[i].y][child_board[i].cmd] += pow(2, MAXDEPTH - depth);
				return score;
			}
		}

		return score;
	}
	else
	{
		score = +INFINITY;

		// generate all possible moves and save them in local array child_board
		for (int x = 0; x <BOARD_SIZE; x++)
		{
			for (int y = 0; y<BOARD_SIZE; y++)
			{
				if (board_operate[x][y] == other_flag)
				{
					for (int cmd = 0; cmd<ALLCMD; cmd++)
					{
						memcpy(board_operate, node_board, sizeof(char)*BOARD_SIZE*BOARD_SIZE);
						bool move_success = false;
						move_success = placeInNodeBoard(board_operate, x, y, cmd, other_flag);

						if (move_success)
						{
							// saved status
							(child_board[child_offset]).x = x;
							(child_board[child_offset]).y = y;
							(child_board[child_offset]).cmd = cmd;
							(child_board[child_offset]).history_score = history_table[x][y][cmd];
							child_offset++;
						}
					}
				}
			}
		}

		// sort nodes by history_score, from high to low
		qsort(child_board, 16 * ALLCMD, sizeof(Child), highHistoryScore);

		for (int i = 0; i< child_offset - 1; i++)
		{
			memcpy(board_operate, node_board, sizeof(char)*BOARD_SIZE*BOARD_SIZE);
			placeInNodeBoard(board_operate, child_board->x, child_board->y, child_board->cmd, other_flag);

			int new_score = (alphaBeta(board_operate, depth - 1, prune_alpha, prune_beta, me_flag));
			score = MIN(score, new_score);
			prune_beta = MIN(prune_beta, score);

			if (prune_beta <= prune_alpha)
			{
				// trigger pruning and increase history_score
				history_table[child_board[i].x][child_board[i].y][child_board[i].cmd] += pow(2, MAXDEPTH - depth);
				return score;
			}
		}

		return score;
	}
}

void initAI(int me) 
{
	memset(history_table, 0, sizeof(int) * BOARD_SIZE *BOARD_SIZE * ALLCMD);
}

struct Command findValidPos(const char board[][BOARD_SIZE], int flag)
{
	char node_board[BOARD_SIZE][BOARD_SIZE] = { 0 };
	memcpy(node_board, board, sizeof(char)*BOARD_SIZE*BOARD_SIZE);

	int x_temp = 0;
	int y_temp = 0;
	int cmd_temp = 0;

	int max_score = -INFINITY;
	int temp_score = 0;

	Sboard child_board[16 * ALLCMD] = { 0 };
	int child_offset = 0;

	for (int x = 0; x <BOARD_SIZE; x++)
	{
		for (int y = 0; y<BOARD_SIZE; y++)
		{
			if (node_board[x][y] == me_flag)
			{
				for (int cmd = 0; cmd<ALLCMD; cmd++)
				{
					memcpy(node_board, board, sizeof(char)*BOARD_SIZE*BOARD_SIZE);
					bool move_success = false;
					move_success = placeInNodeBoard(node_board, x, y, cmd, me_flag);

					if (move_success)
					{
						(child_board[child_offset]).x = x;
						(child_board[child_offset]).y = y;
						(child_board[child_offset]).cmd = cmd;
						(child_board[child_offset]).score = getScore(node_board); 
						child_offset++;
					}
				}
			}
		}
	}
	
	qsort(child_board, 16 * ALLCMD, sizeof(Child), highShallowScore);

	int limit = MIN(FIRST_STEP_LIMIT, child_offset);
	for (int i = 0; i < limit; i++)
	{
		memcpy(node_board, board, sizeof(char)*BOARD_SIZE*BOARD_SIZE);
		placeInNodeBoard(node_board, child_board->x, child_board->y, child_board->cmd, me_flag);

		temp_score = (alphaBeta(node_board, MAXDEPTH, BOOTALPHA, BOOTBETA, other_flag));
		if(temp_score > max_score)
		{
			max_score = temp_score;
			x_temp = child_board[i].x;
			y_temp = child_board[i].y;
			cmd_temp = child_board[i].cmd;
		}
	}

	struct Command command =
	{
		x_temp,
		y_temp,
		cmd_temp
	};

	return command;
}

struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me) {
	
	struct Command preferedPos = findValidPos(board, me);

	return preferedPos;
}

/*
* End Here
*/

BOOL place(int x, int y, OPTION option, int cur_flag) 
{
	if (board[x][y] != cur_flag) 
	{
		return FALSE;
	}

	int new_x = x + DIR[option][0];
	int new_y = y + DIR[option][1];

	if (!isInBound(new_x, new_y) || board[new_x][new_y] != EMPTY) 
	{
		return FALSE;
	}

	board[x][y] = EMPTY;
	board[new_x][new_y] = cur_flag;

	int cur_other_flag = 3 - cur_flag;

	int intervention_dir[4][2] = { { 1, 0 },{ 0, 1 },{ 1, 1 },{ 1, -1 } };
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

	int custodian_dir[8][2] = { { 1, 0 },{ -1, 0 },{ 0, 1 },{ 0, -1 },{ 1, 1 },{ 1, -1 },{ -1, 1 },{ -1, -1 } };

	for (int i = 0; i < 8; i++) 
	{
		int x1 = new_x + custodian_dir[i][0];
		int y1 = new_y + custodian_dir[i][1];
		int x2 = new_x + custodian_dir[i][0] * 2;
		int y2 = new_y + custodian_dir[i][1] * 2;
		if (isInBound(x1, y1) && isInBound(x2, y2) && board[x2][y2] == cur_flag && board[x1][y1] == cur_other_flag) 
		{
			board[x1][y1] = cur_flag;
		}
	}

	return TRUE;
}

void start(int flag) 
{
	memset(board, 0, sizeof(board));

	for (int i = 0; i < 3; i++) 
	{
		board[2][2 + i] = WHITE;
		board[6][6 + i] = WHITE;
		board[5][3 + i] = BLACK;
		board[9][7 + i] = BLACK;
	}

	for (int i = 0; i < 2; i++) 
	{
		board[8 + i][2] = WHITE;
		board[2 + i][9] = BLACK;
	}

	initAI(flag);
}

void turn() 
{
	// AI
	struct Command command = aiTurn((const char(*)[BOARD_SIZE])board, me_flag);
	place(command.x, command.y, command.option, me_flag);
	printf("%d %d %d\n", command.x, command.y, command.option);
	fflush(stdout);
}

void end(int x) 
{

}

void loop() 
{
	while (TRUE)
	{
		memset(buffer, 0, sizeof(buffer));
		gets(buffer);

		if (strstr(buffer, START))
		{
			char tmp[MAX_BYTE] = { 0 };
			sscanf(buffer, "%s %d", tmp, &me_flag);
			other_flag = 3 - me_flag;
			start(me_flag);
			printf("OK\n");
			fflush(stdout);
		}
		else if (strstr(buffer, PLACE))
		{
			char tmp[MAX_BYTE] = { 0 };
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
			char tmp[MAX_BYTE] = { 0 };
			int x;
			sscanf(buffer, "%s %d", tmp, &x);
			end(x);
		}
		else if (strstr(buffer, "PRINT"))
		{
			printBoard();
		}
	}
}


int main(int argc, char *argv[]) 
{
	loop();
	return 0;
}
