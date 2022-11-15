#include<iostream>
#include<cstring>
#include<windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
//#include"resource.h"//导入资源之后有一个resource.h头文件 记得加上
#include<Windows.h>
#include<mmsystem.h>
#include<iomanip>//包含setw()函数
#include<ctime>
#include<cstdlib>//包含srand()函数和rand()函数
#include <graphics.h>
#include <time.h>
#include <conio.h>
#include<cmath>
#include<math.h>
#include<algorithm>
#include <fstream>
#include<stdio.h>
#undef UNICODE
using namespace std;
MOUSEMSG mouse; //定义鼠标的全局变量
int board[25][25];//放棋的数组   0为空 1为黑  2为白  黑棋先手
int r = 1;//回合数
int option = 0;//选择的棋子颜色 我把它置为全局变量 因为后面有个函数要用到
int x = 0, y = 0;//下的棋子坐标 我也置为了全局变量 因为比较重要
int dx[4] = { 0,0,-1,1 };
int dy[4] = { 1,-1,0,0 };
bool lose_directly = false;
//差不多准备用这个  1280x650大小屏幕
int X1 = 0;//鼠标的横纵
int Y1 = 0;
void screen_of_game();
void choose_color();
int main();
void save();
void load();
void game();
void restore();
IMAGE img[5];				// 保存图片
void load_picture() {
	loadimage(&img[0], L"豌豆.png");
	loadimage(&img[1], L"坚果.png");
	mciSendString(_T("open music.mp3 alias mymusic"), NULL, 0, NULL);
	loadimage(&img[2], L"认输.png", 80, 80);
	loadimage(&img[3], L"重来.png", 80, 80);
	loadimage(&img[4], L"存盘.png", 80, 80);
}
void victory() {//胜利界面   
	cleardevice();//清空画布	
	loadimage(NULL, L"victory.png");				//背景图片
}
void defeat() {//胜利界面   
	cleardevice();//清空画布	
	loadimage(NULL, L"defeat.png");				//背景图片
}
bool have_saved = false;
void get_x_and_y() {
	FlushMouseMsgBuffer();// 清空鼠标消息缓冲区
	while (true) {
		mouse = GetMouseMsg();// 获取一个鼠标消息。如果没有，就等待
		if (mouse.uMsg == WM_LBUTTONDOWN)	//是否按鼠标左键
		{
			int xs = mouse.x;
			int ys = mouse.y;
			if (xs >= 700 && xs <= 750 && ys >= 50 && ys <= 100)
			{
				//点了认输
				lose_directly = true;
				defeat();
				while (true) {
					mouse = GetMouseMsg();// 获取一个鼠标消息。如果没有，就等待
					if (mouse.uMsg == WM_LBUTTONDOWN)	//是否按鼠标左键
					{
						return;
					}
				}
			}
			if (xs >= 800 && xs <= 850 && ys >= 50 && ys <= 100)
			{
				//点了重来
				main();
				return;
			}
			if (xs >= 900 && xs <= 950 && ys >= 50 && ys <= 100)
			{
				//点了存盘
				save();
				have_saved = true;
				return;
			}
			if (xs <= 660 && xs >= 100 && ys >= 50 && ys <= 610)
			{
				for (int i = 0; i < 9; i++)

					for (int j = 0; j < 9; j++)
						if (70 * i + 100 - xs <= 20 && xs - (70 * i + 100) <= 20 && 70 * j + 50 - ys <= 20 && ys - (70 * j + 50) <= 20)
						{
							x = i;
							y = j;
							return;
						}

			}//选出了玩家落子xy
		}
	}
}

//游戏说明
void init()
{

	initgraph(1280, 650); // 初始图形化界面，宽高	
	mciSendString(_T("play mymusic"), NULL, 0, NULL);
	loadimage(NULL, L"smy.jpg");				//背景图片
	//给幕布画线
	setlinecolor(BLACK);
	FlushMouseMsgBuffer();// 清空鼠标消息缓冲区

	while (true) {
		mouse = GetMouseMsg();// 获取一个鼠标消息。如果没有，就等待
		if (mouse.uMsg == WM_LBUTTONDOWN)	//是否按鼠标左键
		{
			X1 = mouse.x;
			Y1 = mouse.y;
			if (X1 >= 200 && X1 <= 350 && Y1 >= 510 && Y1 <= 580)//如果点一刀99999级
			{
				victory();
				FlushMouseMsgBuffer();// 清空鼠标消息缓冲区
				while (true) {
					mouse = GetMouseMsg();
					if (mouse.uMsg == WM_LBUTTONDOWN)	//是否按鼠标左键
					{
						init();
						return;
					}
				}
			}
			if (X1 >= 390 && X1 <= 540 && Y1 >= 510 && Y1 <= 580)//如果点新游戏
			{
				choose_color();
				return;
			}
			if (X1 >= 730 && X1 <= 910 && Y1 >= 510 && Y1 <= 580)//如果点读取存档
			{
				load();
				return;

			}
			if (X1 >= 950 && X1 <= 1100 && Y1 >= 510 && Y1 <= 580)//如果点游戏说明
			{
				cleardevice();//清空画布	
				loadimage(NULL, L"不围棋规则.png");
				FlushMouseMsgBuffer();// 清空鼠标消息缓冲区
				while (true) {
					mouse = GetMouseMsg();
					if (mouse.uMsg == WM_LBUTTONDOWN)	//是否按鼠标左键
					{
						init();
						return;
					}
				}
			}
		}
	}
}
//xs=70x+100  ys=70y+50

void screen_of_game() {
	cleardevice();//清空画布	
	loadimage(NULL, L"背景图片.png");
	putimage(700, 50, &img[2]);
	putimage(800, 50, &img[3]);
	putimage(900, 50, &img[4]);
	//restore();
	//画棋盘的线
	setlinecolor(YELLOW);
	//画横线
	for (int i = 0; i < 9; i++)
		line(100, 50 + 70 * i, 660, 50 + 70 * i);
	for (int i = 0; i < 9; i++)
		line(100 + 70 * i, 50, 100 + 70 * i, 610);
}

void delay() {
	for (int i = 0; i < 10000; i++) {
		for (int j = 0; j < 10000; j++) {
			for (int k = 0; k < 12; k++) {
			}
		}
	}
}
int has_found[9][9] = { 0 };//用于判断气的时候看是否找过(x,y)处，若找过了，则是1，否则是0
//判断边界
inline bool inBorder(int x, int y)
{
	return x >= 0 && x < 9 && y >= 0 && y < 9;
}
//应用气：如果下了一个子，导致旁边的对方棋子没有气，输；
//如果下了一个子，这个子所处位置无气，自杀输

//判断(x,y)处的棋子是否有气 
bool has_air(int x, int y)
{
	has_found[x][y] = true;
	bool flag = false;
	for (int dir = 0; dir < 4; dir++)
	{
		int temp_x = x + dx[dir], temp_y = y + dy[dir];
		if (inBorder(temp_x, temp_y))
		{
			if (board[temp_x][temp_y] == 0)
				flag = true;
		}
	}
	for (int dir = 0; dir < 4; dir++)
	{
		int temp_x = x + dx[dir], temp_y = y + dy[dir];
		if (inBorder(temp_x, temp_y))
		{
			if (board[temp_x][temp_y] == board[x][y] && !has_found[temp_x][temp_y])
				if (has_air(temp_x, temp_y))
					flag = true;
		}
	}
	return flag;
}
//判断自杀  true 自杀   false活着 
bool suicide(int x, int y)
{
	if (!has_air(x, y))
	{
		memset(has_found, 0, sizeof(has_found));
		return true;
	}
	else
	{
		memset(has_found, 0, sizeof(has_found));
		return false;
	}
}

//判断是否吃子   false  吃子    true 没吃子 
bool not_eat_opponent(int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		int temp_x = x + dx[i];
		int temp_y = y + dy[i];
		if (inBorder(temp_x, temp_y) && board[temp_x][temp_y] != 0 && board[temp_x][temp_y] != board[x][y])
		{
			if (!has_air(temp_x, temp_y))
			{
				memset(has_found, 0, sizeof(has_found));
				return false;
			}
		}
	}
	memset(has_found, 0, sizeof(has_found));
	return true;
}

//判断下了这一步后输了没有   
//判断(x,y)处是否是颜色为color 的棋子的合法位置   true合法(下了之后不输） false不合法 （下了就死了）
bool legal_step(int x, int y, int color)//在还没有把color色的棋子下在这里，
//判断(x,y)处是否是颜色为color的棋子的合法位置
{
	bool ans = false;
	if (board[x][y] == 0)
	{
		board[x][y] = color;
		if (!suicide(x, y) && not_eat_opponent(x, y))
			ans = true;
		board[x][y] = 0;
		memset(has_found, 0, sizeof(has_found));
	}
	return ans;
}
//判断一个位置周围的在边界内的位置有几个  
int calc_dir(int fx, int fy) {
	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
		int temp_x = fx + dx[i], temp_y = fy + dy[i];
		if (inBorder(temp_x, temp_y)) ++cnt;
	}
	return cnt;
}
//判断一个位置四周的在边界内的位置上有几个col色的棋子  
int check_cross(int fx, int fy, int col) {
	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
		int cx = fx + dx[i], cy = fy + dy[i];
		if (inBorder(cx, cy)) {
			if (board[cx][cy] == col) ++cnt;
		}
	}
	return cnt;
}
int eyelist[10][10] = { 0 };
//判断是否是电脑人工智能已经形成的眼，目的是最后在把自己形成的眼填上  
void eye()
{
	for (int x = 0; x < 9; x++)
		for (int y = 0; y < 9; y++)
		{
			if (board[x][y] == 0)
			{
				int cnt = 0;
				for (int i = 0; i < 4; i++)
				{
					int xx = x + dx[i];
					int yy = y + dy[i];
					if (inBorder(xx, yy) && board[xx][yy] == 3 - option)
						cnt++;
				}
				if (cnt == calc_dir(x, y))
					eyelist[x][y] = 3;
			}
		}
}
//将游戏初始化 如棋盘清空 回合数清零
void origin() {
	memset(board, 0, sizeof(board));
	r = 1;
}
//打出棋盘


//检查下的某一步是否合法(符合游戏规则）
bool checkstep(int x, int y) {
	if (x < 0 || y < 0 || x >= 9 || y >= 9)
		return false;
	if (board[x][y] != 0)
		return false;
	if (x == 4 && y == 4 && r == 1)//黑子先手不能下中间
		return false;
	return true;
}
void choose_color() {
	cleardevice();//清空画布	
	loadimage(NULL, L"选黑白.png");
	FlushMouseMsgBuffer();// 清空鼠标消息缓冲区
	while (true) {
		mouse = GetMouseMsg();// 获取一个鼠标消息。如果没有，就等待
		if (mouse.uMsg == WM_LBUTTONDOWN)	//是否按鼠标左键
		{
			X1 = mouse.x;
			Y1 = mouse.y;
			if (X1 >= 10 && X1 <= 120 && Y1 >= 90 && Y1 <= 200)
			{
				option = 1;
				return;
			}
			if (X1 >= 140 && X1 <= 250 && Y1 >= 90 && Y1 <= 200)
			{
				option = 2;
				return;
			}
		}
	}
}

//静态局面评估函数 得到x，y点的价值

int valuepoint[9][9] = { 0 };
int significance[9][9];
//给significance赋值 
void give_significance() {

	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			significance[i][j] = 1;
}
//选点,每一个新的手数把valuepoint归零  评估每个点的value 
int give_valuepoint(int x, int y, int color)
{
	int value = 0;
	if (checkstep(x, y))
	{//如果这里能下(空着的)
		board[x][y] = color;// 就下
		if (suicide(x, y) || !not_eat_opponent(x, y)) {//如果这一步下了就死了
			value = -10000;
			board[x][y] = 0;
			return value;
		}
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
			{
				if (!legal_step(i, j, 3 - color))//如果这里能下 并且下了电脑会死
					value += significance[i][j];
			}
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
			{
				if (!legal_step(i, j, color))//这里能下并且人下了会死
					value -= significance[i][j];
			}
		board[x][y] = 0;
	}
	valuepoint[x][y] = value;
	return value;
}
//计算曼哈顿距离 
int manhattan_d(int x1, int y1, int x2, int y2)
{
	int d = abs(x1 - x2) + abs(y1 - y1);
	return d;
}

void scatter() {
	int cnt = 0;//打散规则中符合条件的总点数
	int x_scatter[81], y_scatter[81];
	int max1 = 0;
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			if (!legal_step(i, j, 3 - option) || !checkstep(i, j))
				continue;
			else//它就是可下子  选出它和棋盘中所有点的曼哈顿距离的最小值
			{
				int min1 = 20;
				for (int ii = 0; ii < 9; ii++)
					for (int jj = 0; jj < 9; jj++) {
						if (manhattan_d(i, j, ii, jj) < min1)
							min1 = manhattan_d(i, j, ii, jj);
					}//min1是对于（i，j）点来说曼哈顿距离的最小值
				if (min1 > max1) {//曼哈顿距离最小的最大值的点
					cnt = 0;
					max1 = min1;
					x_scatter[cnt] = i;
					y_scatter[cnt] = j;
					cnt++;
				}
				if (min1 == max1) {//曼哈顿距离最小的最大值的点
					x_scatter[cnt] = i;
					y_scatter[cnt] = j;
					cnt++;
				}

			}
		}
	if (cnt == 1)
	{
		x = x_scatter[0];
		y = y_scatter[0];
	}
	if (cnt > 1) {
		int k = 0;
		srand((unsigned)time(NULL));//用系统时间产生随机数种子
		k = rand() % cnt;//在0到cnt-1中选一个数
		x = x_scatter[k];
		y = y_scatter[k];
	}
}
//minmax搜索
int maxi = 2;//层数

void give_maxivalue()
{
	if (r <= 30)
		maxi = 1;
	else if (r >= 50)
		maxi = 3;
	else
		maxi = 2;
}
int temp_x1, temp_y1;

int dfs(int depth, int color, int number)//层数 子的颜色 值 颜色
{
	give_maxivalue();
	if (depth > maxi)//搜到了最底层，返回
	{
		return give_valuepoint(temp_x1, temp_y1, color);
	}
	int ret = 0, flag = 0;
	if (color == 3 - option) {
		//如果是max层
		ret = -2147483647, flag = 0;
	}
	if (color == option) {
		//如果是min层
		ret = 2147483647, flag = 0;
	}
	//int ret = 0, flag = 0;//初值不一定都是0，根据当前min还是max层来定
	if (color == 3 - option)//在max层 
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (legal_step(i, j, color))//枚举落子点并落子
				{
					temp_x1 = i;
					temp_y1 = j;
					ret = max(ret, dfs(depth + 1, 3 - color, ret));//对于子节点的返回值取max
					if (ret > number)//剪枝
					{
						flag = 1;
						break;
					}
				}
				if (flag == 1)
					break;
			}
			return ret;
		}
	if (color == option)//在min层
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (legal_step(i, j, color))//枚举落子点并落子
				{
					temp_x1 = i;
					temp_y1 = j;
					ret = min(ret, dfs(depth + 1, 3 - color, ret));//对于子节点的返回值取max
					if (ret < number)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 1)
					break;
			}
			return ret;
		}
}
bool flag1 = 0;//用于判断是否堵对方的眼
void blockopponenteye()//堵住对方将要造出的眼  
{
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			if (legal_step(i, j, 3 - option)) {
				if (check_cross(i, j, option) == calc_dir(i, j) - 1) {
					x = i;
					y = j;
					flag1 = 1;
				}
			}
			if (flag1) break;
		}
		if (flag1) break;
	}
}

bool ans1 = false;//用于判断是否造了自己的眼
void makeeye()//手动造眼  
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (board[i][j] == 0)
			{
				int cnt1 = 0;
				int cnt2 = 0;
				int x0 = 0;
				int y0 = 0;
				for (int k = 0; k < 4; k++)
				{
					int xx = i + dx[k];
					int yy = j + dy[k];
					if (inBorder(xx, yy) && board[xx][yy] == 3 - option)//自己的棋子个数
						cnt1++;
					if (inBorder(xx, yy) && board[xx][yy] == 0)//空位
					{
						cnt2++;
						x0 = xx;
						y0 = yy;
					}
				}
				if (cnt1 == calc_dir(i, j) - 1 && cnt2 == 1 && legal_step(x0, y0, 3 - option))
				{
					ans1 = true;
					x = x0;
					y = y0;
					break;
				}
			}
		}
		if (ans1 == true)
			break;
	}
}
//人工智能

//通过价值评估函数来选择电脑落子的地方
//int point_of_maxvalue[81][9][9];// 价值最大的点
struct point {
	int x;
	int y;
};
point point_of_maxvalue[81];
int cnt_of_maxvalue = 0;
bool findnoteye = false;

void putchess_of_AI() {//电脑的颜色就是3-option
	if (r == 1)
	{
		srand((unsigned)time(NULL));//用系统时间产生随机数种子
		x = rand() % 9;
		y = rand() % 9;
		while (!checkstep(x, y)) {
			srand((unsigned)time(NULL));//用系统时间产生随机数种子
			x = rand() % 9;
			y = rand() % 9;
		}
	}
	else {
		blockopponenteye();
		if (!flag1)//没有堵对方的眼
		{
			makeeye();//去自己造眼
			if (!ans1)//自己没有造成眼
			{
				eye();
				int maxvalue = -2147483648;
				cnt_of_maxvalue = 0;
				for (int i = 0; i < 9; i++)
					for (int j = 0; j < 9; j++) {//遍历每个点的价值
						if (checkstep(i, j) && legal_step(i, j, 3 - option) && eyelist[i][j] != 3) {//如果这个点可以下而且不是我造的眼
							findnoteye = true;//还有可以不用把自己的眼堵上的位置
							int temp_max = dfs(0, 3 - option, 2147483647);
							if (temp_max > maxvalue) {
								cnt_of_maxvalue = 0;
								maxvalue = temp_max;
								point_of_maxvalue[cnt_of_maxvalue].x = i;
								point_of_maxvalue[cnt_of_maxvalue].y = j;
								cnt_of_maxvalue++;
							}
							if (temp_max == maxvalue) {
								point_of_maxvalue[cnt_of_maxvalue].x = i;
								point_of_maxvalue[cnt_of_maxvalue].y = j;
								cnt_of_maxvalue++;
							}
						}

					}
				if (cnt_of_maxvalue == 1)
				{
					x = point_of_maxvalue[cnt_of_maxvalue - 1].x;
					y = point_of_maxvalue[cnt_of_maxvalue - 1].y;//选定了电脑下的x y
				}
				if (cnt_of_maxvalue > 1)
				{
					//执行打散规则
					scatter();
					//选出了电脑下的x y
				}
				if (!findnoteye)
					for (int i = 0; i < 9; i++)
						for (int j = 0; j < 9; j++)
							if (eyelist[i][j] == 3)
							{
								x = i;
								y = j;
							}
			}
		}
	}
}

//打散来选AI的落子

void save()
{
	ofstream fout("temp.txt");
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
			fout << board[i][j] << " ";
		fout << endl;
	}
	fout << option << " ";
	fout << r;
	fout.close();
}

void load() {
	ifstream fin("temp.txt");
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			fin >> board[i][j];
	fin >> option;
	fin >> r;
	//fin.close();
}

void restore() {
	//复原存过的局面
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			X1 = 70 * i + 100;
			Y1 = 70 * j + 50;
			if (board[i][j] == 1)
			{
				putimage(X1 - 30, Y1 - 30, &img[0]);
			}
			if (board[i][j] == 2)
			{
				putimage(X1 - 30, Y1 - 30, &img[1]);

			}
		}
}

void game() {
	screen_of_game();
	restore();
	while (true) {
		if (r % 2 == 2 - option) {//如果轮到自己下
			lose_directly = false;//是否点了认输
			have_saved = false;
			get_x_and_y();
			//如果要下的话		
			if (lose_directly)
				return;
			if (have_saved)
				return;
			while (!checkstep(x, y)) {
				get_x_and_y();
			}
		}
		if (checkstep(x, y)) {//坐标合法
			board[x][y] = option;
			X1 = 70 * x + 100;
			Y1 = 70 * y + 50;
			if (option == 1)
			{
				putimage(X1 - 30, Y1 - 30, &img[0]);
			}
			if (option == 2)
			{
				putimage(X1 - 30, Y1 - 30, &img[1]);

			}
			if (suicide(x, y) || !not_eat_opponent(x, y))
			{
				//输掉的界面
				defeat();
				FlushMouseMsgBuffer();// 清空鼠标消息缓冲区
				while (true) {
					mouse = GetMouseMsg();// 获取一个鼠标消息。如果没有，就等待
					if (mouse.uMsg == WM_LBUTTONDOWN)	//是否按鼠标左键
					{
						return;
					}
				}
			}
			r++;//回合数加一

		}
		if (r % 2 != option) {//如果轮到对手下
			/*srand((unsigned)time(NULL));//用系统时间产生随机数种子
			x = rand() % 9;
			y = rand() % 9;
			while (!checkstep(x, y)) {
				srand((unsigned)time(NULL));//用系统时间产生随机数种子
				x = rand() % 9;
				y = rand() % 9;
			}*/
			putchess_of_AI();//得到了x, y ;
			ans1 = false;
			flag1 = 0;
			memset(eyelist, 0, sizeof(eyelist));
			board[x][y] = 3 - option;
			X1 = 70 * x + 100;
			Y1 = 70 * y + 50;
			if (option == 1)
				putimage(X1 - 30, Y1 - 30, &img[1]);
			if (option == 2)
				putimage(X1 - 30, Y1 - 30, &img[0]);
			if (suicide(x, y) || !not_eat_opponent(x, y))
			{
				//电脑输了 即你赢了
				victory();
				while (true) {
					mouse = GetMouseMsg();// 获取一个鼠标消息。如果没有，就等待
					if (mouse.uMsg == WM_LBUTTONDOWN)	//是否按鼠标左键
					{
						return;
					}
				}
			}
			r++;//回合数加一
		}
	}

}
int main() {
	load_picture();
	init();
	game();

	return 0;
}

