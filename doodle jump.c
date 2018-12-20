// doodle jump.cpp: �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>

//�궨��
#define WIDTH 480
#define HEIGHT 600
#define G 1
#define V -20

const int player_width = 100;//��Ҹ߶�
const int player_height = 100;//��ҿ��
const int board_width = 100;//����߶�
const int board_height = 50;//������
const int sleeptime = 50;//ÿ�θ��¼��ʱ��
const int board_number = 30;//��������

//ȫ�ֱ���
float position_x, position_y;
float velocity_x, velocity_y;
IMAGE background;//����ͼƬ
IMAGE player_left, player_right;//��ҳ���ͼƬ����ҳ���ͼƬ
IMAGE left_cover, right_cover;//��������ͼ����������ͼ
IMAGE normal_board;//��ͨ����
IMAGE normal_cover;//��ͨ��������ͼ

struct Node {
	float x, y;

}board[board_number];

//��������
void Menu();
void Startup();
void ShowPlayer();
void MovePlayer();
void ShowBoard();
void MoveBoard();
void MoveDown();
bool isOnBoard();
void PutNewBoard();
void GameOver();

int main()
{
	Menu();
	Startup();
	ShowPlayer();
	ShowBoard();
	while (1)
	{
		/*UpdateWithInput();
		UpdateWithoutInput();
		*/
		MoveBoard();
		MovePlayer();
		FlushBatchDraw();//�����������ƣ���ֹ������˸
		Sleep(sleeptime);//�������ֹͣ
	}
	EndBatchDraw();
	getchar();
    return 0;
}

//��������

//�˵�
void Menu()
{
	initgraph(WIDTH,HEIGHT);
	
}

//��ʼ��
void Startup()
{
	srand(time(NULL));//���������
	//����ͼƬ
	loadimage(&background, "background.jpg", WIDTH, HEIGHT);
	loadimage(&player_right, "player_right.jpg", player_width, player_height);
	loadimage(&player_left, "player_left.jpg", player_width, player_height);
	loadimage(&right_cover, "right_cover.jpg", player_width, player_height);
	loadimage(&left_cover, "left_cover.jpg", player_width, player_height);
	loadimage(&normal_board, "normal_board.jpg", board_width,board_height);
	loadimage(&normal_cover, "normal_cover.jpg", board_width, board_height);
	//��ʼ����ҵ�λ�ú��ٶ�
	position_x = WIDTH / 2 - player_width / 2;
	position_y = HEIGHT - player_height + player_height * 0.15;
	velocity_x = 0;
	velocity_y = V;
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
	//�������ͼƬ
	putimage(position_x, position_y, &right_cover, NOTSRCERASE);//����ͼ
	putimage(position_x, position_y, &player_right,SRCINVERT);//ԭͼ
}

//��ʾ����
void ShowBoard()
{
	int i;
	int high = 0;//����߶�
	for (i = 0; ; i++)
	{
		if (high > HEIGHT - board_height)
		{
			break;
		}
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
	for (i = 0; i < 12; i++)
	{
		//ÿ�θ��¶��ػ����壬��ֹ�����ͼƬ�ڸ�
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
			board[i].y = 0 - board_height;//��������������ܸ߶�
			//��������ͼƬ
			putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
		}
	}
}

bool isOnBoard()
{
	//TODO
	return true;
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
		if (board[i].y < 0 || board[i].y > HEIGHT)
		{
			continue;
		}
		if (board[i].y > lowest)
		{
			lowest = board[i].y;
		}
	}
	move_dis = lowest - (position_y + player_height);//���ƾ���Ϊ������������������֮��
	//ʵ��100�����������ƵĶ���
	while (cnt < 10)
	{
		position_x += move_dis;
		for (i = 0; i < board_number; i++)
		{
			board[i].y += move_dis;
		}
		Sleep(10);
		cnt++;
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
