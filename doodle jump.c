
// doodle jump.cpp: �������̨Ӧ�ó������ڵ㡣
//

//#include "pch.h"
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
#define G 0.2
#define V -10

const int player_width = 100; //��ҿ��
const int player_height = 100; //��Ҹ߶�
const int board_width = 100; //������
const int board_height = 50; //����߶�
const int button_width = 130;
const int button_height = 60;
const int sleeptime = 8; //ÿ�θ��¼��ʱ��
const int board_number = 20; //��������

//ȫ�ֱ���
float position_x, position_y;
float velocity_x, velocity_y;
float diff_x, diff_y;
float highest = 600;//��ߵ�����ĸ߶�
float lowest = 0;
int state;//��ҵ�״̬
int score_pre, score_now;//��߷�/��ǰ�÷�
bool jumped;//��������ʶ
char ch; //�����ַ�
FILE* fp;
IMAGE background; //����ͼƬ
IMAGE menuback;//�˵�����
IMAGE endback;//�������汳��
IMAGE player_left, player_right; //��ҳ���ͼƬ����ҳ���ͼƬ
IMAGE left_cover, right_cover; //��������ͼ����������ͼ
IMAGE normal_board; //��ͨ����
IMAGE normal_cover; //��ͨ��������ͼ
IMAGE spring_board;//�Ĵ�����
IMAGE spring_cover;//�Ĵ���������ͼ
IMAGE play;//��ʼ��Ϸ��ť
IMAGE rule;//��Ϸ����ť
IMAGE playagain;//������Ϸ��ť
IMAGE exit1;//�˳���ť
IMAGE button_cover;//��ť����ͼ

typedef enum Player
{
	RIGHT,
	LEFT,
} player_state;
typedef enum Board
{
	normal,
	move,
	spring
} board_type;

struct Node
{
	float x, y;
	board_type type;
} board[board_number];

//��������
void LoadImg(); //����ͼƬ
void LoadInfo();//���ط���
void Menu(); //�˵�
void ShowRule(); //��Ϸ����
void Startup(); //��ʼ��
void MovePlayer(); //�ƶ����
void ChangeDir(); //�ı䷽��
void DoubleJump();//������
bool isOnBoard(); //�ж��Ƿ�̤������
bool isDrop(); //�ж��Ƿ�׹��
void ShowBoard(); //��ʾ����
void MoveBoard(); //�ƶ�����
void PutNewBoard();//����������
void MoveDown(); //��������
void PrintScore(); //��ӡ����
void SaveInfo();//�������
bool DoNext(); //��һ������
void GameOver(); //��Ϸ����
void Ending(); //��������

int main()
{
	LoadImg();
	Menu();
begin:
	LoadInfo();
	Startup();
	BeginBatchDraw();
	ShowBoard();
	while (1)
	{
		if (_kbhit())
			ch = _getch();
		ChangeDir();
		DoubleJump();
		ch = '0';
		if (isOnBoard())
		{
			MoveDown();
		}
		MoveBoard();
		MovePlayer();
		if (isDrop())
		{
			break;
		}
		PrintScore();
		FlushBatchDraw(); //�����������ƣ���ֹ������˸
		Sleep(sleeptime); //�������ֹͣ
	}
	GameOver();
	EndBatchDraw();
	Ending();
	if (DoNext())
		goto begin;
	closegraph();
	return 0;
}

//��������

void LoadImg()
{
	//����ͼƬ
	loadimage(&background, "background1.jpg", WIDTH, HEIGHT);
	loadimage(&menuback, "menuback.jpg", WIDTH, HEIGHT);
	loadimage(&endback, "gameover.jpg", WIDTH, HEIGHT);
	loadimage(&player_right, "player_right.jpg", player_width, player_height);
	loadimage(&player_left, "player_left.jpg", player_width, player_height);
	loadimage(&right_cover, "right_cover.jpg", player_width, player_height);
	loadimage(&left_cover, "left_cover.jpg", player_width, player_height);
	loadimage(&normal_board, "normal_board.jpg", board_width, board_height);
	loadimage(&normal_cover, "normal_cover.jpg", board_width, board_height);
	loadimage(&spring_board, "spring_board.jpg", board_width, board_height);
	loadimage(&spring_cover, "spring_cover.jpg", board_width, board_height);
	loadimage(&play, "play.jpg", button_width, button_height);
	loadimage(&button_cover, "button_cover.jpg", button_width, button_height);
	loadimage(&rule, "rule.jpg", button_width, button_height);
	loadimage(&playagain, "playagain.jpg", button_width, button_height);
	loadimage(&exit1, "exit.jpg", button_width, button_height);
}

//���ط���
void LoadInfo()
{
	fp = fopen("HighScore.txt", "r");
	if (fp == NULL)
	{
		printf("Please create \"HighScore.txt\" first!\n");
	}
	fscanf_s(fp, "%d", &score_pre);
	fclose(fp);
}

//�˵�
void Menu()
{
	initgraph(WIDTH, HEIGHT);
	putimage(0, 0, &menuback);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT / 2, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT / 2, &play, SRCINVERT);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT *5 / 8, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT *5 / 8, &rule, SRCINVERT);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT * 3 / 4, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT * 3 / 4, &exit1, SRCINVERT);
	while (true)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)//����������
		{
			if (m.x >= WIDTH / 2 - board_width / 2 && m.x <= WIDTH / 2 - board_width / 2 + button_width && m.y >= HEIGHT / 2 && m.y <= HEIGHT / 2 + button_height)
			{
				return;//��ʼ��Ϸ
			}
			else if (m.x >= WIDTH / 2 - board_width / 2 && m.x <= WIDTH / 2 - board_width / 2 + button_width && m.y >= HEIGHT * 5 / 8 && m.y <= HEIGHT * 5 / 8 + button_height)
			{
				ShowRule();//��ʾ����
				return;
			}
			else if (m.x >= WIDTH / 2 - board_width / 2 && m.x <= WIDTH / 2 - board_width / 2 + button_width && m.y >= HEIGHT * 3 / 4 && m.y <= HEIGHT * 3 / 4 + button_height)
			{
				exit(0);//�˳���Ϸ
			}
		}	
	}
}

//��ʼ��
void Startup()
{
	srand(time(NULL)); //���������
					   //��ʼ����ҵ�λ�ú��ٶ�
	position_x = WIDTH / 2 - player_width / 2;
	position_y = HEIGHT - player_height + player_height * 0.15;
	velocity_x = 0;
	velocity_y = V;
	score_now = 0;
	highest = 0;
}

//�ƶ����
void MovePlayer()
{
	//���̤����������»ָ����ϳ��ٶ�
	if (position_x < 0 - player_width)
	{
		position_x = WIDTH; //������������߽磬�ʹ��ұ߽����
	}
	else if (position_x > WIDTH)
	{
		position_x = 0; //�����������ұ߽磬�ʹ���߽����
	}
	//������ҵ��ٶȺ�λ��
	velocity_y = velocity_y + G; //v = v0 + gt
	position_y = position_y + velocity_y; // y = y0 + vt
	position_x = position_x + velocity_x;
	//�������ͼƬ
	switch (state)
	{
	case RIGHT:
		putimage(position_x, position_y, &right_cover, NOTSRCERASE); //����ͼ
		putimage(position_x, position_y, &player_right, SRCINVERT); //ԭͼ
		break;
	case LEFT:
		putimage(position_x, position_y, &left_cover, NOTSRCERASE); //����ͼ
		putimage(position_x, position_y, &player_left, SRCINVERT); //ԭͼ
		break;
	}
}

//��ʾ����
void ShowBoard()
{
	int type_num; //������������������
	for (int i = 0; i < board_number; i++)
	{
		board[i].x = rand() % (WIDTH * 3 / 4); //���������������꣬��ʹ�俿����Ļ�в�
		board[i].y = i * board_height;
		type_num = rand() % 100;//���������������
		if (type_num < 5)
		{
			board[i].type = spring;//�Ĵ�����
			putimage(board[i].x, board[i].y, &spring_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &spring_board, SRCINVERT);
		}
		else if (type_num < 20)
		{
			board[i].type = move;//�ƶ�����
			putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
		}
		else
		{
			board[i].type = normal;//��ͨ����
			putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
		}
	}
}

//�ƶ�����
void MoveBoard()
{
	putimage(0, 0, &background); //�ñ���ͼƬ�ڸ����������ƶ��ĺۼ�
	for (int i = 0; i < board_number; i++)
	{
		if (board[i].y < 0 || board[i].y > HEIGHT) //������岻����Ļ���򲻸���
		{
			continue;
		}
		//ÿ�θ��¶��ػ����壬��ֹ�����ͼƬ�ڸ�
		if (board[i].type == move)
		{
			board[i].x += 2;
			if (board[i].x > WIDTH)//���������ұ߽磬�����߽����
			{
				board[i].x = -board_width;
			}
		}
		switch (board[i].type)
		{
		case normal:
		case move:
			putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
			break;
		case spring:
			putimage(board[i].x, board[i].y, &spring_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &spring_board, SRCINVERT);
			break;
		}
	}
}

//����������
void PutNewBoard()
{
	int i;
	for (i = 0; i < board_number; i++)
	{
		if (board[i].y > HEIGHT)
		{
			board[i].x = rand() % (WIDTH * 3 / 4); //���������������꣬��ʹ�俿����Ļ�в�
			board[i].y = highest - board_height; //��������������ܸ߶�
			highest = board[i].y;
			//��������ͼƬ
			switch (board[i].type)
			{
			case normal:
			case move:
				putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
				putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
				break;
			case spring:
				putimage(board[i].x, board[i].y, &spring_cover, NOTSRCERASE);
				putimage(board[i].x, board[i].y, &spring_board, SRCINVERT);
				break;
			}
		}
	}
}

//�ж��Ƿ��������
bool isOnBoard()
{
	int i;
	for (i = 0; i < board_number; i++)
	{
		diff_x = position_x - board[i].x;								//������ĺ������
		diff_y = position_y + player_height - board[i].y;	//��������������
															//���̤������
		if (velocity_y >= 0 && diff_y >= 30 && diff_y <= 50 && diff_x >= -board_width * 2 / 5 && diff_x <= board_width * 2 / 5)
		{
			velocity_y = V;//��ֱ�ٶ�����Ϊ���ٶ�
			if (board[i].type == spring)
			{
				velocity_y = V * 1.5;//����ǱĴ�������ٶȸ���
			}
			jumped = 0;//���ö��������
			velocity_x = 0;//����ˮƽ�ٶ�
			return true;
		}
	}
	return false;
}

//��������
void MoveDown()
{
	int i, cnt = 0; //��ʱ��
	//float lowest = 0; //�����������������
	float move_dis; //�������Ƶľ���
	//Ѱ�������������������������
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
	PutNewBoard();
	move_dis = lowest - (position_y + player_height); //���ƾ���Ϊ������������������֮��
	if (move_dis < 0)//��ֹ��������
	{
		move_dis = 0;
	}
	score_now += move_dis;//�ӷ�
	if (score_now > score_pre)//���������߷֣������
	{
		score_pre = score_now;
	}
	//ʵ�������ƵĶ���
	while (cnt < 10)
	{
		putimage(0, 0, &background);
		position_y += move_dis / 10;
		switch (state)
		{
		case RIGHT:
			putimage(position_x, position_y, &right_cover, NOTSRCERASE); //����ͼ
			putimage(position_x, position_y, &player_right, SRCINVERT); //ԭͼ
			break;
		case LEFT:
			putimage(position_x, position_y, &left_cover, NOTSRCERASE); //����ͼ
			putimage(position_x, position_y, &player_left, SRCINVERT); //ԭͼ
			break;
		}
		for (i = 0; i < board_number; i++)
		{
			board[i].y += move_dis / 10;
			switch (board[i].type)
			{
			case normal:
			case move:
				putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
				putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
				break;
			case spring:
				putimage(board[i].x, board[i].y, &spring_cover, NOTSRCERASE);
				putimage(board[i].x, board[i].y, &spring_board, SRCINVERT);
				break;
			}
		}
		FlushBatchDraw();
		Sleep(sleeptime);
		cnt++;
	}
}

//��ӡ����
void PrintScore()
{
	settextstyle(20, 10, "Arial");
	/*
	outtextxy(WIDTH / 2 - 80 , 10, "Score:");
	outtextxy(WIDTH / 2 - 80, 40, "Highest Score:");
	char s1[10];
	char s2[10];
	sprintf_s(s1, "%d", score_now);
	outtextxy(WIDTH / 2 - 20, 10, s1);
	sprintf_s(s2, "%d", score_pre);
	outtextxy(WIDTH / 2 + 60, 40, s2);
	*/
	outtextxy(WIDTH / 2 - 80, 10, "highest:");
	outtextxy(WIDTH / 2 - 80, 40, "lowest:");
	char s1[10];
	char s2[10];
	sprintf_s(s1, "%d", highest);
	outtextxy(WIDTH / 2 + 60, 10, s1);
	sprintf_s(s2, "%d", lowest);
	outtextxy(WIDTH / 2 + 60, 40, s2);
}

//�ı䷽��
void ChangeDir()
{
	switch (ch)
	{
	case 'A':
	case 'a':
		velocity_x = -7;
		state = LEFT;
		break;
	case 'D':
	case 'd':
		velocity_x = 7;
		state = RIGHT;
		break;
	default:
		break;
	}
}

//��Ϸ��������
void GameOver()
{
	for (int i = 0; i < board_number; i++)
	{
		board[i].y = 0;
	}
	position_y = 0;
	for (int cnt = 0; cnt < 100; cnt++)
	{
		putimage(0, 0, &background);
		velocity_y = velocity_y + G / 2;
		position_y = position_y + velocity_y;
		switch (state)
		{
		case RIGHT:
			putimage(position_x, position_y, &right_cover, NOTSRCERASE); //����ͼ
			putimage(position_x, position_y, &player_right, SRCINVERT); //ԭͼ
			break;
		case LEFT:
			putimage(position_x, position_y, &left_cover, NOTSRCERASE); //����ͼ
			putimage(position_x, position_y, &player_left, SRCINVERT); //ԭͼ
			break;
		}
		FlushBatchDraw();
		cnt++;
		Sleep(sleeptime);
	}
}

//�ж��Ƿ�׹��
bool isDrop()
{
	if (position_y > HEIGHT)//������Ļ�׶�
		return true;
	return false;
}

//��ʾ����
void ShowRule()
{
	putimage(0, 0, &background);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT * 3 / 4, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT * 3 / 4, &play, SRCINVERT);
	settextstyle(20, 10, "Arial");
	outtextxy(WIDTH  / 3, HEIGHT *2 / 5, "Tribute to Doodle Jump.");
	outtextxy(WIDTH  / 3, HEIGHT / 2, "Press A and D to move.");
	outtextxy(WIDTH  / 3, HEIGHT / 2 + 20, "Press W for double jump.");
	outtextxy(WIDTH  / 3, HEIGHT / 2 + 40, "If you drop, you will die.");
	outtextxy(WIDTH / 3, HEIGHT / 2 + 60, "Have fun.");
	while (1) 
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			if (m.x >= WIDTH / 2 - board_width / 2 && m.x <= WIDTH / 2 - board_width / 2 + button_width && m.y >= HEIGHT * 3 / 4 && m.y <= HEIGHT * 3 / 4 + button_height)
			{
				break;
			}
		}
	}
	return;
}

//��������
void Ending()
{
	putimage(0, 0, &endback);
	settextstyle(20, 10, "Arial");
	outtextxy(WIDTH / 2 - 80, 250, "Score:");
	outtextxy(WIDTH / 2 - 80, 280, "Highest Score:");
	char s1[10];
	char s2[10];
	sprintf_s(s1, "%d", score_now);
	outtextxy(WIDTH / 2 - 20, 250, s1);
	sprintf_s(s2, "%d", score_pre);
	outtextxy(WIDTH / 2 + 60, 280, s2);
	SaveInfo();                        //�����浵
	putimage(WIDTH / 2 - board_width / 2, HEIGHT * 5 / 8, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT * 5 / 8, &playagain, SRCINVERT);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT * 3 / 4, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - board_width / 2, HEIGHT * 3 / 4, &exit1, SRCINVERT);
	while (true)
	{
		MOUSEMSG m;
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {
			//���"playagain"��ť
			if (m.x >= WIDTH / 2 - board_width / 2 && m.x <= WIDTH / 2 - board_width / 2 + button_width && m.y >= HEIGHT * 5 / 8 && m.y <= HEIGHT * 5 / 8 + button_height)
			{
				ch = '1';
				
				return;
			}
			//���"exit"��ť
			else if (m.x >= WIDTH / 2 - board_width / 2 && m.x <= WIDTH / 2 - board_width / 2 + button_width && m.y >= HEIGHT * 3 / 4 && m.y <= HEIGHT * 3 / 4 + button_height)
			{
				ch = '2';
				return;
			}
			else {
				continue;
			}
		}
	}
	
}

//������
void DoubleJump()
{
	if (ch == 'w' && !jumped)
	{
		velocity_y = V;
		jumped = 1;//���Ϊ�Ѿ�����һ��
	}
}

//ִ����һ��
bool DoNext()
{
	if (ch == '1')
		return true;
	else if (ch == '2')
		exit(0);
	else
		Ending();
}

//�������
void SaveInfo()
{
	if (score_now > score_pre)
	{
		fp = fopen("HighScore.txt", "w");
		fprintf_s(fp, "%d", score_now);
	}
	else
	{
		fp = fopen("HighScore.txt", "w");
		fprintf_s(fp, "%d", score_pre);
	}
	fclose(fp);
}



