
// doodle jump.cpp: 定义控制台应用程序的入口点。
//

//#include "pch.h"
#include "stdafx.h"
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//宏定义
#define WIDTH 480
#define HEIGHT 600
#define G 0.5
#define V -15

const int player_width = 100; //玩家宽度
const int player_height = 100; //玩家高度
const int board_width = 100; //跳板宽度
const int board_height = 50; //跳板高度
const int button_width = 130;
const int button_height = 60;
const int sleeptime = 20; //每次更新间隔时间
const int board_number = 200; //跳板数量

							  //全局变量
float position_x, position_y;
float velocity_x, velocity_y;
float diff_x, diff_y;
float highest;//最高的跳板的高度
int state;//玩家的状态
int score_pre,score_now;//当前得分
int highscore;//最高分
bool jumped;//二段跳标识
char ch; //输入字符
IMAGE background; //背景图片
IMAGE player_left, player_right; //玩家朝左图片，玩家朝右图片
IMAGE left_cover, right_cover; //朝左遮罩图，朝右遮罩图
IMAGE normal_board; //普通跳板
IMAGE normal_cover; //普通跳板遮罩图
IMAGE spring_board;//蹦床跳板
IMAGE spring_cover;//蹦床跳板遮罩图
IMAGE play;//开始游戏按钮
IMAGE rule;//游戏规则按钮
IMAGE playagain;//重新游戏按钮
IMAGE exit1;//退出按钮
IMAGE button_cover;//按钮遮罩图

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

//函数声明
void LoadImg(); //加载图片
void Menu(); //菜单
void ShowRule(); //游戏规则
void Startup(); //初始化
void MovePlayer(); //移动玩家
void ChangeDir(); //改变方向
void DoubleJump();
void ShowBoard(); //显示跳板
void MoveBoard(); //移动跳板
void MoveDown(); //整体下移
void PutNewBoard();//生成新跳板
void PrintScore(); //打印分数
bool isOnBoard(); //判断是否踏上跳板
bool isDrop(); //判断是否坠落
void GameOver(); //游戏结束
void Ending(); //结束界面
void SaveInfo();
bool DoNext(); //下一步操作

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
		FlushBatchDraw(); //进行批量绘制，防止出现闪烁
		Sleep(sleeptime); //程序短暂停止
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

//函数定义

void LoadImg()
{
	//载入图片
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

//菜单
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

//初始化
void Startup()
{
	srand(time(NULL)); //重设随机数
					   //初始化玩家的位置和速度
	position_x = WIDTH / 2 - player_width / 2;
	position_y = HEIGHT - player_height + player_height * 0.15;
	velocity_x = 0;
	velocity_y = V;
	score = 0;
	highest = 0;
	BeginBatchDraw(); //开始批量绘制
}

//移动玩家
void MovePlayer()
{
	if (position_x < 0 - player_width)
	{
		position_x = WIDTH; //如果玩家跳出左边界，就从右边界出现
	}
	else if (position_x > WIDTH)
	{
		position_x = 0; //如果玩家跳出右边界，就从左边界出现
	}
	//更新玩家的速度和位置
	velocity_y = velocity_y + G; //v = v0 + gt
	position_y = position_y + velocity_y; // y = y0 + vt
	position_x = position_x + velocity_x;
	//放置玩家图片
	switch (state)
	{
	case RIGHT:
		putimage(position_x, position_y, &right_cover, NOTSRCERASE); //遮罩图
		putimage(position_x, position_y, &player_right, SRCINVERT); //原图
		break;
	case LEFT:
		putimage(position_x, position_y, &left_cover, NOTSRCERASE); //遮罩图
		putimage(position_x, position_y, &player_left, SRCINVERT); //原图
		break;
	}
}

//显示跳板
void ShowBoard()
{
	int type_num; //生成跳板种类的随机数
	for (int i = 0; i < board_number; i++)
	{
		board[i].x = 90 + rand() % (WIDTH * 5 / 8); //随机生成跳板横坐标，并使其靠近屏幕中部
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
	putimage(0, 0, &background); //用背景图片掩盖所有物体移动的痕迹
	for (int i = 0; i < board_number; i++)
	{
		if (board[i].y < 0 || board[i].y > HEIGHT) //如果跳板不在屏幕内则不更新
		{
			continue;
		}
		//每次更新都重绘跳板，防止被玩家图片掩盖

		if (board[i].type == move)
		{
			board[i].x += 5;
			if (board[i].x > WIDTH)//如果跳板出右边界，则从左边界出现
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
			board[i].x = 90 + rand() % (WIDTH * 5 / 8); //随机生成跳板横坐标，并使其靠近屏幕中部
			board[i].y = highest - board_height; //跳板纵坐标等于总高度
			highest = board[i].y;
			//放置跳板图片
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

//整体下移
void MoveDown()
{
	int i, cnt = 0; //计时器
	float lowest = 0; //最下面跳板的纵坐标
	float move_dis; //整体下移的距离
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
	move_dis = lowest - (position_y + player_height); //下移距离为玩家与最下面跳板距离之差
	if (move_dis < 0)
	{
		move_dis = 0;
	}
	score += move_dis;//加分
	//实现100毫秒内逐渐下移的动画

	while (cnt < 10)
	{
		putimage(0, 0, &background);
		position_y += move_dis / 10;
		switch (state)
		{
		case RIGHT:
			putimage(position_x, position_y, &right_cover, NOTSRCERASE); //遮罩图
			putimage(position_x, position_y, &player_right, SRCINVERT); //原图
			break;
		case LEFT:
			putimage(position_x, position_y, &left_cover, NOTSRCERASE); //遮罩图
			putimage(position_x, position_y, &player_left, SRCINVERT); //原图
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
			putimage(position_x, position_y, &right_cover, NOTSRCERASE); //遮罩图
			putimage(position_x, position_y, &player_right, SRCINVERT); //原图
			break;
		case LEFT:
			putimage(position_x, position_y, &left_cover, NOTSRCERASE); //遮罩图
			putimage(position_x, position_y, &player_left, SRCINVERT); //原图
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


