// doodle jump.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>

//宏定义
#define WIDTH 480
#define HEIGHT 600
#define G 0.5
#define V -15

const float player_width = 100;//玩家高度
const float player_height = 100;//玩家宽度
const float board_width = 100;//跳板高度
const float board_height = 50;//跳板宽度
const int sleeptime = 20;//每次更新间隔时间
const int board_number = 200;//跳板数量

//全局变量
float position_x, position_y;
float velocity_x, velocity_y;
float high_diff = 0;
float highest;
int state;
IMAGE background;//背景图片
IMAGE player_left, player_right;//玩家朝左图片，玩家朝右图片
IMAGE left_cover, right_cover;//朝左遮罩图，朝右遮罩图
IMAGE normal_board;//普通跳板
IMAGE normal_cover;//普通跳板遮罩图

typedef enum Player { RIGHT, LEFT, SHOOT } player_state;
//typedef enum Board { normal, move } board_type;

struct Node {
	float x, y;
	//board_type type;
}board[board_number];

//函数声明
void Menu();
void Startup();
void ShowPlayer();
void MovePlayer();
void ShowBoard();
void MoveBoard();
void MoveDown();
bool isOnBoard();
void PutNewBoard();
bool isOnBoard();
void ChangeDir();
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
		ChangeDir();
		if (isOnBoard())
		{
			velocity_y = V;
			MoveDown();
		}
		MoveBoard();
		MovePlayer();
		PutNewBoard();
		FlushBatchDraw();//进行批量绘制，防止出现闪烁
		Sleep(sleeptime);//程序短暂停止
	}
	EndBatchDraw();
	getchar();
    return 0;
}

//函数定义

//菜单
void Menu()
{
	initgraph(WIDTH,HEIGHT);
	
}

//初始化
void Startup()
{
	srand(time(NULL));//重设随机数
	//载入图片
	loadimage(&background, "background.jpg", WIDTH, HEIGHT);
	loadimage(&player_right, "player_right.jpg", player_width, player_height);
	loadimage(&player_left, "player_left.jpg", player_width, player_height);
	loadimage(&right_cover, "right_cover.jpg", player_width, player_height);
	loadimage(&left_cover, "left_cover.jpg", player_width, player_height);
	loadimage(&normal_board, "normal_board.jpg", board_width,board_height);
	loadimage(&normal_cover, "normal_cover.jpg", board_width, board_height);
	//初始化玩家的位置和速度
	position_x = WIDTH / 2 - player_width / 2;
	position_y = HEIGHT - player_height + player_height * 0.15;
	velocity_x = 0;
	velocity_y = V;
	BeginBatchDraw();//开始批量绘制
}

//显示玩家
void ShowPlayer()
{
	putimage(position_x, position_y, &player_left);
}

//移动玩家
void MovePlayer()
{
	//本段代码测试用，正式版本应修改为判断是否踏上跳板
	//玩家落地后重新恢复向上初速度
	if (position_y >= HEIGHT - player_height * 0.9)//玩家的脚底不位于图片最底部
	{
		velocity_y = V;
	}
	if (position_x < 0 - player_width)
	{
		position_x = WIDTH;//如果玩家跳出左边界，就从右边界出现
	}
	else if (position_x > WIDTH)
	{
		position_x = 0;//如果玩家跳出右边界，就从左边界出现
	}
	//更新玩家的速度和位置
	velocity_y = velocity_y + G;//v = v0 + gt
	position_y = position_y + velocity_y;// y = y0 + vt
	position_x = position_x + velocity_x;
	velocity_x = 0;
	//放置玩家图片
	switch (state) {
	case RIGHT:
		putimage(position_x, position_y, &right_cover, NOTSRCERASE);//遮罩图
		putimage(position_x, position_y, &player_right, SRCINVERT);//原图
		break;
	case LEFT:
		putimage(position_x, position_y, &left_cover, NOTSRCERASE);//遮罩图
		putimage(position_x, position_y, &player_left, SRCINVERT);//原图
		break;

	}
	
}

//显示跳板
void ShowBoard()
{
	int i;
	int high = 0;//跳板高度
	for (i = 0; i < board_number; i++)
	{
		board[i].x = 90 + rand() % (WIDTH * 5 / 8);//随机生成跳板横坐标，并使其靠近屏幕中部
		board[i].y = high;//跳板纵坐标等于总高度
		//放置跳板图片
		putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
		putimage(board[i].x, board[i].y, &normal_board,SRCINVERT);
		high += board_height ;//总高度每次增加一个跳板的高度
	}
}

void MoveBoard()
{
	putimage(0, 0, &background);//用背景图片掩盖所有物体移动的痕迹
	int i;
	for (i = 0; i < board_number; i++)
	{
		//每次更新都重绘跳板，防止被玩家图片掩盖
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
			board[i].x = 90 + rand() % (WIDTH * 5 / 8);//随机生成跳板横坐标，并使其靠近屏幕中部
			board[i].y = highest - board_height;//跳板纵坐标等于总高度
			highest = board[i].y;
			//board[i].type = move;
			//放置跳板图片
			putimage(board[i].x, board[i].y, &normal_cover, NOTSRCERASE);
			putimage(board[i].x, board[i].y, &normal_board, SRCINVERT);
		}
	}
}


//整体下移
void MoveDown()
{
	int i, cnt = 0;			//计时器
	float lowest = 0;	//最下面跳板的纵坐标
	float move_dis;		//整体下移的距离
	//寻找最下面的跳板
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
	move_dis = lowest - (position_y + player_height);//下移距离为玩家与最下面跳板距离之差
	if (move_dis < 0)
	{
		move_dis = 0;
	}
	//实现100毫秒内逐渐下移的动画
	
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
	//TODO
	return false;
}

void ChangeDir()
{
	//TODO
}

void GameOver()
{
	int cnt = 0; //计时器
	//玩家下坠动画
	while (cnt < 60)
	{
		velocity_y += G;
		Sleep(sleeptime);
		cnt++;
	}
}

