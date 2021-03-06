#include<iostream>
#include<cstring>
#include<bits/stdc++.h>
#include "jsoncpp/json.h"
using namespace std;

#define inf 1000000000
#define MaxDepth 2

int board[9][9] = { 0 };		//定义记录棋盘情况的数组,黑子代号为1，白子为-1 
bool visited_by_air_judging[9][9] = { 0 };		//对已访问过的位置进行标记 
int available_list_x[81], available_list_y[81];
int value_list[9][9];
int maxvalue_x[81], maxvalue_y[81];
//定义某点的上下左右位移的四种状态 
int rowmove[4] = { 1,0,-1,0 };
int colmove[4] = { 0,1,0,-1 };
int my_colour;
int rev_colour(int colour)
{
	if (colour == 1)return -1;
	else return 1;
}
//判断某点（由棋盘数组的下标确定）是否在棋盘内 
bool inboard_judging(int x, int y)
{
	if (x >= 0 && x <= 8 && y >= 0 && y <= 8)return 1;
	else return 0;
}
//判断点（x,y）处是否有气 
bool air_judging(int x, int y)
{
	bool flag = 0;			//定义是否有气的判定布尔变量 
	visited_by_air_judging[x][y] = 1;		//标记该点，避免重复访问造成死循环 
	for (int i = 0; i < 4; i++)
	{
		int nextx = x + rowmove[i];			//定义上下左右四种位移后得到的新坐标 
		int nexty = y + colmove[i];
		if (inboard_judging(nextx, nexty))		//这个点必须在棋盘内 
		{
			if (board[nextx][nexty] == 0)flag = 1;		//如果该点为空，则(x,y)有气 
			else if (board[nextx][nexty] == board[x][y] && visited_by_air_judging[nextx][nexty] == 0)		//满足条件：该点棋子与（x,y）同色且未被访问过（避免死循环） 
			{
				if (air_judging(nextx, nexty) == 1)			//该点有气，则(x,y)也有气 
				{
					flag = 1;
				}
			}
		}
	}
	return flag;
}
//判断点（x,y）处能否落子（1表示可以落子，-1表示该点有棋子，-2表示自杀落子，-3表示将对手的棋围住） 
int available_judging(int x, int y, int colour)		//x,y为坐标，colour为检验落子的颜色代码（黑为1，白为-1） 
{
	if (board[x][y] != 0)return 0;			//如果该点已有棋子，则不能落子 
	board[x][y] = colour;			//先将该点的落子标记 
	memset(visited_by_air_judging, 0, sizeof(visited_by_air_judging));		//初始化访问记录数组
	if (air_judging(x, y) == 0)		//如果该点没气，则不能落子
	{
		board[x][y] = 0;		//恢复该点棋盘情况 
		return 0;
	}
	for (int i = 0; i < 4; i++)			//考察该点上下左右四个点的气的情况 
	{
		int nextx = x + rowmove[i];			//定义上下左右四种位移后得到的新坐标 
		int nexty = y + colmove[i];
		if (inboard_judging(nextx, nexty) == 1)		//如果该点在棋盘内则继续讨论 
		{
			if (board[nextx][nexty] != 0 && visited_by_air_judging[nextx][nexty] == 0)			//考察有棋子的位置（记录访问避免死循环） 
			{
				if (air_judging(nextx, nexty) == 0)		//如果在（x,y）落子会导致其四周的棋子没气，则不能落子 
				{
					board[x][y] = 0;		//恢复该点棋盘情况 
					return 0;
				}
			}
		}
	}
	board[x][y] = 0;		//恢复该点棋盘情况 
	return 1;
}
bool patch(int x, int y, int colour)
{
	for (int i = 0; i < 4; i++)
	{
		int xx = x + rowmove[i];
		int yy = y + colmove[i];
		if (board[xx][yy] == colour)return 1;
	}
	return 0;
}
//对当前局面进行评估，计算颜色为colour的一方可落子的位置数目 
int evaluate(int colour)
{
	int ans = 0;
	int revcolour = rev_colour(colour);
	for (int x = 0; x < 9; x++)
	{
		for (int y = 0; y < 9; y++)
		{
			if (available_judging(x, y, colour) == 1)ans++;
			if (available_judging(x, y, revcolour) == 1)ans--;
		}
	}
	return ans;
}
//判断当前局面对颜色为colour的一方的优势度 

int main()
{
	srand((unsigned)time(0));
	string str;
	int x, y;
	// 读入JSON
	getline(cin, str);
	//getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	// 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID = input["responses"].size();			//trunID为自己已走的步数
	for (int i = 0; i < turnID; i++)
	{
		x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();		//requests为对方的过往输入
		if (x != -1) board[x][y] = 1;
		x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();			//responses为己方的过往输出
		if (x != -1) board[x][y] = -1;
	}
	x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
	if (x != -1) board[x][y] = 1;
	// 输出决策JSON
	Json::Value ret;
	Json::Value action;

	//以下为策略 
	memset(available_list_x, 0, sizeof(available_list_x));
	memset(available_list_y, 0, sizeof(available_list_y));
	memset(value_list, 0, sizeof(value_list));
	int colour = x == -1 ? 1 : -1;
	int AI_available_judge = 0;
	int max_value = -inf;
	int max_number = 0;
	for (int xx = 0; xx < 9; xx++)
	{
		for (int yy = 0; yy < 9; yy++)
		{
			if (available_judging(xx, yy, colour) == 1)
			{
				available_list_x[AI_available_judge] = xx;
				available_list_y[AI_available_judge] = yy;
				AI_available_judge++;
				board[xx][yy] = colour;
				value_list[xx][yy] = evaluate(colour);
				if (patch(xx, yy, colour))value_list[xx][yy]--;
				if (value_list[xx][yy] > max_value)
				{
					max_value = value_list[xx][yy];
				}
				board[xx][yy] = 0;
			}
			else
			{
				value_list[xx][yy] = -inf - 1;
			}
		}
	}
	for (int xx = 0; xx < 9; xx++)
	{
		for (int yy = 0; yy < 9; yy++)
		{
			if (value_list[xx][yy] == max_value)
			{
				maxvalue_x[max_number] = xx;
				maxvalue_y[max_number] = yy;
				max_number++;
			}
		}
	}
	int position = rand() % max_number;
	int decide_x = maxvalue_x[position];
	int decide_y = maxvalue_y[position];

	action["x"] = decide_x; action["y"] = decide_y;
	ret["response"] = action;
	Json::FastWriter writer;

	cout << writer.write(ret) << endl;
	return 0;
}