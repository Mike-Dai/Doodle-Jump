// doodle jump.cpp: �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//�궨��
#define WIDTH 480
#define HEIGHT 600
#define G 0.5
#define V -15

const int player_width = 100;//��Ҹ߶�
const int player_height = 100;//��ҿ��
const int board_width = 100;//����߶�
const int board_height = 50;//������
const int sleeptime = 20;//ÿ�θ��¼��ʱ��
const int board_number = 200;//��������

//ȫ�ֱ���
float position_x, position_y;
float velocity_x, velocity_y;
float high_diff = 0;
float highest;
int state;
int score;
int highscore;
IMAGE background;//����ͼƬ
IMAGE player_left, player_right;//��ҳ���ͼƬ����ҳ���ͼƬ
IMAGE left_cover, right_cover;//��������ͼ����������ͼ
IMAGE normal_board;//��ͨ����
IMAGE normal_cover;//��ͨ��������ͼ

typedef enum Player { RIGHT, LEFT, SHOOT } player_state;
//typedef enum Board { normal, move } board_type;

struct Node {
	float x, y;
	//board_type type;
}board[board_number];

//��������
void LoadImg();
void Menu();
void Rule();
void Startup();
void ShowPlayer();
void MovePlayer();
void ShowBoard();
void MoveBoard();
void MoveDown();
bool isOnBoard();
void PutNewBoard();
bool isOnBoard();
void PrintScore();
void ChangeDir();
void GameOver();
bool isDrop();
void ShowRule();
void UpdateWithInput();
void Ending();

int main()
{
	LoadImg();
	Menu();
begin:
	Startup();
	ShowPlayer();
	ShowBoard();
	while (1)
	{
		if (_kbhit())
		{
			UpdateWithInput();
		}
		if (isOnBoard())
		{
			velocity_y = V;
			MoveDown();
		}
		PrintScore();
		MoveBoard();
		MovePlayer();
		PutNewBoard();
		if (isDrop())
		{
			GameOver();
			break;
		}
		FlushBatchDraw();//�����������ƣ���ֹ������˸
		Sleep(sleeptime);//�������ֹͣ
	}
	EndBatchDraw();
	goto begin;
    return 0;
}

//��������

void LoadImg()
{
	//����ͼƬ
	loadimage(&background, "background.jpg", WIDTH, HEIGHT);
	loadimage(&player_right, "player_right.jpg", player_width, player_height);
	loadimage(&player_left, "player_left.jpg", player_width, player_height);
	loadimage(&right_cover, "right_cover.jpg", player_width, player_height);
	loadimage(&left_cover, "left_cover.jpg", player_width, player_height);
	loadimage(&normal_board, "normal_board.jpg", board_width, board_height);
	loadimage(&normal_cover, "normal_cover.jpg", board_width, board_height);
}

//�˵�
void Menu()
{
	initgraph(WIDTH,HEIGHT);
	putimage(0, 0, &background);
	outtextxy(WIDTH / 2, HEIGHT / 3, "1.play");
	outtextxy(WIDTH / 2, HEIGHT * 2 / 5, "2.rule");
	outtextxy(WIDTH / 2, HEIGHT / 2, "choose 1 or 2");
	char input;
	input = _getch();
	switch (input)
	{
	case '1':
		return;
	case '2':
		Rule();
	default:
		//Menu();
		return;
	}
	
}

void Rule()
{
	//TODO
}

//��ʼ��
void Startup()
{
	srand(time(NULL));//���������
	//��ʼ����ҵ�λ�ú��ٶ�
	position_x = WIDTH / 2 - player_width / 2;
	position_y = HEIGHT - player_height + player_height * 0.15;
	velocity_x = 0;
	velocity_y = V;
	score = 0;
	BeginBatchDraw();//��ʼ��������
}

//��ʾ���
void ShowPlayer()
{
	putimage(position_x, position_y, &player_left);
}

//�ƶ����
void MovePlayer()
{
	//���δ�������ã���ʽ�汾Ӧ�޸�Ϊ�ж��Ƿ�̤������
	//�����غ����»ָ����ϳ��ٶ�
	if (position_y >= HEIGHT - player_height * 0.9)//��ҵĽŵײ�λ��ͼƬ��ײ�
	{
		velocity_y = V;
	}
	if (position_x < 0 - player_width)
	{
		position_x = WIDTH;//������������߽磬�ʹ��ұ߽����
	}
	else if (position_x > WIDTH)
	{
		position_x = 0;//�����������ұ߽磬�ʹ���߽����
	}
	//������ҵ��ٶȺ�λ��
	velocity_y = velocity_y + G;//v = v0 + gt
	position_y = position_y + velocity_y;// y = y0 + vt
	position_x = position_x + velocity_x;
	velocity_x = 0;
	//�������ͼƬ
	switch (state) {
	case RIGHT:
		putimage(position_x, position_y, &right_cover, NOTSRCERASE);//����ͼ
		putimage(position_x, position_y, &player_right, SRCINVERT);//ԭͼ
		break;
	case LEFT:
		putimage(position_x, position_y, &left_cover, NOTSRCERASE);//����ͼ
		putimage(position_x, position_y, &player_left, SRCINVERT);//ԭͼ
		break;

	}
	
}

//��ʾ����
void ShowBoard()
{
	int i;
	int high = 0;//����߶�
	for (i = 0; i < board_number; i++)
	{
		board[i].x = 90 + rand() % (WIDTH * 5 / 8);//���������������꣬��ʹ�俿����Ļ�в�
		board[i].y = high;//��������������ܸ߶�
		//��������ͼƬ
		putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
		putimage(board[i].x, board[i].y, &normal_board,SRCINVERT);
		high += board_height ;//�ܸ߶�ÿ������һ������ĸ߶�
	}
}

void MoveBoard()
{
	putimage(0, 0, &background);//�ñ���ͼƬ�ڸ����������ƶ��ĺۼ�
	int i;
	for (i = 0; i < board_number; i++)
	{
		//ÿ�θ��¶��ػ����壬��ֹ�����ͼƬ�ڸ�
		/*
		if (board[i].type == move)
		{
			board[i].x += 5;
		}
		*/
		putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
		putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
	}
}

void PutNewBoard()
{
	int i;
	for (i = 0; i < board_number; i++)
	{
		if (board[i].y > HEIGHT)
		{
			board[i].x = 90 + rand() % (WIDTH * 5 / 8);//���������������꣬��ʹ�俿����Ļ�в�
			board[i].y = highest - board_height;//��������������ܸ߶�
			highest = board[i].y;
			//board[i].type = move;
			//��������ͼƬ
			putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
		}
	}
}


//��������
void MoveDown()
{
	int i, cnt = 0;			//��ʱ��
	float lowest = 0;	//�����������������
	float move_dis;		//�������Ƶľ���
	//Ѱ�������������
	for (i = 0; i < board_number; i++)
	{
		if (board[i].y > HEIGHT)
		{
			continue;
		}
		if (board[i].y > lowest)
		{
			lowest = board[i].y;
		}
		if (board[i].y < highest)
		{
			highest = board[i].y;
		}
	}
	move_dis = lowest - (position_y + player_height);//���ƾ���Ϊ������������������֮��
	if (move_dis < 0)
	{
		move_dis = 0;
	}
	//ʵ��100�����������ƵĶ���
	
	while (cnt < 10)
	{
		position_y += move_dis / 10;
		for (i = 0; i < board_number; i++)
		{
			board[i].y += move_dis / 10;
		}
		Sleep(10);
		cnt++;
	}
}

bool isOnBoard()
{
	for (int i = 0; i < board_number; i++)
	{
		if (position_y - board[i].y >= 0 && fabs(position_x - board[i].x) < 5 * 10e-1)
			return true;
	}
	return false;
}

void PrintScore()
{
	outtextxy(WIDTH / 2, 0, "score:");
	char s[10];
	sprintf(s, "%d", score);
	outtextxy(WIDTH / 2 + 10, 0, s);
}

void ChangeDir()
{
	
	if (_getch() == 'A')
	{
		if (velocity_x > 0)
			velocity_x *= -1.0;
	}
	if (_getch() == 'D')
	{
		if (velocity_x < 0)
			velocity_x *= -1.0;
	}
}

void GameOver()
{
	int cnt = 0; //��ʱ��
	//�����׹����
	while (cnt < 60)
	{
		velocity_y += G;
		Sleep(sleeptime);
		cnt++;
	}
}

bool isDrop()
{
	for (int i = 0; i < board_number; i++)
	{
		if (board[i].y < position_y&&fabs(board[i].x - position_x) < 5 * 10e-1)
			return false;
	}
	return true;
}

void ShowRule()
{
	outtext("Tribute to Doodle Jump. Press A to move left and D to move right. Have fun");
}

void UpdateWithInput()
{
	ChangeDir();
}

void Ending()
{
	putimage(0, 0, &background);
	outtext("GAME OVER!");
	outtext("1.play again");
	outtext("2.exit");
	outtext("choose 1 or 2");
	char input;
	input = _getch();
	switch (input)
	{
	case '1':
		break;
	case '2':
		exit(0);
	default:
		Ending();
	}

}
