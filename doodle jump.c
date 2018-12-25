
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
#define G 0.5
#define V -15

const int player_width = 100; //��ҿ��
const int player_height = 100; //��Ҹ߶�
const int board_width = 100; //������
const int board_height = 50; //����߶�
const int button_width = 130;
const int button_height = 60;
const int sleeptime = 20; //ÿ�θ��¼��ʱ��
const int board_number = 200; //��������

							  //ȫ�ֱ���
float position_x, position_y;
float velocity_x, velocity_y;
float diff_x, diff_y;
float highest;//��ߵ�����ĸ߶�
int state;//��ҵ�״̬
int score_pre,score_now;//��ǰ�÷�
int highscore;//��߷�
bool jumped;//��������ʶ
char ch; //�����ַ�
IMAGE background; //����ͼƬ
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
void Menu(); //�˵�
void ShowRule(); //��Ϸ����
void Startup(); //��ʼ��
void MovePlayer(); //�ƶ����
void ChangeDir(); //�ı䷽��
void DoubleJump();
void ShowBoard(); //��ʾ����
void MoveBoard(); //�ƶ�����
void MoveDown(); //��������
void PutNewBoard();//����������
void PrintScore(); //��ӡ����
bool isOnBoard(); //�ж��Ƿ�̤������
bool isDrop(); //�ж��Ƿ�׹��
void GameOver(); //��Ϸ����
void Ending(); //��������
void SaveInfo();
bool DoNext(); //��һ������

int main()
{
	LoadImg();
	Menu();
	/*
	ch = _getch();
	if (ch == '2')
		ShowRule();
	else
	*/
		begin:
	Startup();
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
		PrintScore();
		MoveBoard();
		MovePlayer();
		PutNewBoard();
		if (isDrop())
		{
			break;
		}
		FlushBatchDraw(); //�����������ƣ���ֹ������˸
		Sleep(sleeptime); //�������ֹͣ
	}
	GameOver();
	EndBatchDraw();
	Sleep(2000);
	Ending();
	ch = _getch();
	if (DoNext())
		goto begin;
	closegraph();
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
	loadimage(&spring_board, "spring_board.jpg", board_width, board_height);
	loadimage(&spring_cover, "spring_cover.jpg", board_width, board_height);
	loadimage(&play, "play.jpg", button_width, button_height);
	loadimage(&button_cover, "button_cover.jpg", button_width, button_height);
	loadimage(&rule, "rule.jpg", button_width, button_height);
	loadimage(&playagain, "playagain.jpg", button_width, button_height);
	loadimage(&exit1, "exit.jpg", button_width, button_height);
}

//�˵�
void Menu()
{
	initgraph(WIDTH, HEIGHT);
	putimage(0, 0, &background);
	putimage(WIDTH / 2 - 10, HEIGHT / 2, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - 10, HEIGHT / 2, &play, SRCINVERT);
	putimage(WIDTH / 2 - 10, HEIGHT * 3 / 4, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - 10, HEIGHT * 3 / 4, &rule, SRCINVERT);
	while (true)
	{
		MOUSEMSG m;
		m = GetMouseMsg();

		if (m.x >= WIDTH / 2 - 10 && m.x <= WIDTH / 2 - 10 + button_width && m.y >= HEIGHT / 2 - button_height && m.y <= HEIGHT / 2)
		{
			return;
		}
		else if (m.x >= WIDTH / 2 -10 && m.x <= WIDTH / 2 - 10 + button_width && m.y >= HEIGHT * 3 / 4 - button_height && m.y <= HEIGHT * 3 / 4)
		{
			ShowRule();
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
	score = 0;
	highest = 0;
	BeginBatchDraw(); //��ʼ��������
}

//�ƶ����
void MovePlayer()
{
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
		board[i].x = 90 + rand() % (WIDTH * 5 / 8); //���������������꣬��ʹ�俿����Ļ�в�
		board[i].y = i * board_height;
		type_num = rand() % 10;
		if (type_num < 1)
		{
			board[i].type = spring;
			putimage(board[i].x, board[i].y, &spring_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &spring_board, SRCINVERT);
		}
		else if (type_num < 4)
		{
			board[i].type = move;
			putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
		}
		else
		{
			board[i].type = normal;
			putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
		}

	}
}

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
			board[i].x += 5;
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

void PutNewBoard()
{
	int i;
	for (i = 0; i < board_number; i++)
	{
		if (board[i].y > HEIGHT)
		{
			board[i].x = 90 + rand() % (WIDTH * 5 / 8); //���������������꣬��ʹ�俿����Ļ�в�
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

//��������
void MoveDown()
{
	int i, cnt = 0; //��ʱ��
	float lowest = 0; //�����������������
	float move_dis; //�������Ƶľ���
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
	move_dis = lowest - (position_y + player_height); //���ƾ���Ϊ������������������֮��
	if (move_dis < 0)
	{
		move_dis = 0;
	}
	score += move_dis;//�ӷ�
	//ʵ��100�����������ƵĶ���

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
		Sleep(sleeptime / 3);
		cnt++;
	}
}

bool isOnBoard()
{
	int i;
	for (i = 0; i < board_number; i++)
	{
		diff_x = position_x - board[i].x;
		diff_y = position_y + player_height - board[i].y;
		if (velocity_y >= 0 && diff_y >= 40 && diff_y <= 50 && diff_x >= -board_width / 2 && diff_x <= board_width / 2)
		{
			velocity_y=V;
			if (board[i].type == spring)
			{
				velocity_y = V * 1.5;
			}
			jumped = 0;
			velocity_x = 0;
			return true;
		}
	}
	return false;
}

void PrintScore()
{
	outtextxy(WIDTH / 2, HEIGHT / 2, "score:");
	char s[10];
	sprintf_s(s, "%d", score);
	outtextxy(WIDTH / 2 + 10, HEIGHT / 2, s);
}

void ChangeDir()
{
	switch (ch)
	{
	case 'a':
		velocity_x = -10;
		state = LEFT;
		break;
	case 'd':
		velocity_x = 10;
		state = RIGHT;
		break;
	default:
		break;
	}
}

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

bool isDrop()
{
	if (position_y > HEIGHT)
		return true;
	return false;
}

void ShowRule()
{
	putimage(0, 0, &background);
	outtextxy(WIDTH  / 3, HEIGHT *2 / 5, "Tribute to Doodle Jump.");
	outtextxy(WIDTH  / 3, HEIGHT / 2, "Press A and D to move.");
	outtextxy(WIDTH  / 3, HEIGHT / 2 + 20, "Press W to jump.");
	outtextxy(WIDTH  / 3, HEIGHT / 2 + 40, "If you drop, you will die.");
}

void Ending()
{
	putimage(0, 0, &background);
	
	outtextxy(WIDTH / 3, HEIGHT * 2 / 5, "GAME OVER!");
	outtextxy(WIDTH / 3, HEIGHT / 2, "1.play again");
	outtextxy(WIDTH / 3, HEIGHT / 2 + 20, "2.exit");
	outtextxy(WIDTH / 3, HEIGHT / 2 + 40, "choose 1 or 2");
	/*
	putimage(WIDTH / 2 - 10, HEIGHT / 2, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - 10, HEIGHT / 2, &playagain, SRCINVERT);
	putimage(WIDTH / 2 - 10, HEIGHT * 3 / 4, &button_cover, NOTSRCERASE);
	putimage(WIDTH / 2 - 10, HEIGHT * 3 / 4, &exit1, SRCINVERT);
	while (true)
	{
		putimage(0, 0, &background);
		MOUSEMSG m;
		m = GetMouseMsg();

		if (m.x >= WIDTH / 2 - 10 && m.x <= WIDTH / 2 - 10 + button_width && m.y >= HEIGHT / 2 - button_height && m.y <= HEIGHT / 2)
		{
			return;
		}
		else if (m.x >= WIDTH / 2 - 10 && m.x <= WIDTH / 2 - 10 + button_width && m.y >= HEIGHT * 3 / 4 - button_height && m.y <= HEIGHT * 3 / 4)
		{
			exit(0);
		}
	}
	Sleep(2000);
	getchar();
	*/
}

void DoubleJump()
{
	if (ch == 'w' && !jumped)
	{
		velocity_y = V;
		jumped = 1;
	}
}

bool DoNext()
{
	if (ch == '1')
		return true;
	if (ch == '2')
		exit(0);
}


void SaveInfo()
{
	fp = fopen("HighScore.txt", "r");
	fscanf_s(fp, "%d", &score_pre);
	if (score_now > score_pre)
	{
		fclose(fp);
		fp = fopen("HighScore.txt", "w");
		fprintf_s(fp, "%d", score_now);
	}
	fclose(fp);
}


