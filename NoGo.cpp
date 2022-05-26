#include<iostream>
#include<cstring>
#include<bits/stdc++.h>
#include "jsoncpp/json.h"
using namespace std;

#define inf 1000000000
#define MaxDepth 2

int board[9][9] = { 0 };		//�����¼�������������,���Ӵ���Ϊ1������Ϊ-1 
bool visited_by_air_judging[9][9] = { 0 };		//���ѷ��ʹ���λ�ý��б�� 
int available_list_x[81], available_list_y[81];
int value_list[9][9];
int maxvalue_x[81], maxvalue_y[81];
//����ĳ�����������λ�Ƶ�����״̬ 
int rowmove[4] = { 1,0,-1,0 };
int colmove[4] = { 0,1,0,-1 };
int my_colour;
int rev_colour(int colour)
{
	if (colour == 1)return -1;
	else return 1;
}
//�ж�ĳ�㣨������������±�ȷ�����Ƿ��������� 
bool inboard_judging(int x, int y)
{
	if (x >= 0 && x <= 8 && y >= 0 && y <= 8)return 1;
	else return 0;
}
//�жϵ㣨x,y�����Ƿ����� 
bool air_judging(int x, int y)
{
	bool flag = 0;			//�����Ƿ��������ж��������� 
	visited_by_air_judging[x][y] = 1;		//��Ǹõ㣬�����ظ����������ѭ�� 
	for (int i = 0; i < 4; i++)
	{
		int nextx = x + rowmove[i];			//����������������λ�ƺ�õ��������� 
		int nexty = y + colmove[i];
		if (inboard_judging(nextx, nexty))		//���������������� 
		{
			if (board[nextx][nexty] == 0)flag = 1;		//����õ�Ϊ�գ���(x,y)���� 
			else if (board[nextx][nexty] == board[x][y] && visited_by_air_judging[nextx][nexty] == 0)		//�����������õ������루x,y��ͬɫ��δ�����ʹ���������ѭ���� 
			{
				if (air_judging(nextx, nexty) == 1)			//�õ���������(x,y)Ҳ���� 
				{
					flag = 1;
				}
			}
		}
	}
	return flag;
}
//�жϵ㣨x,y�����ܷ����ӣ�1��ʾ�������ӣ�-1��ʾ�õ������ӣ�-2��ʾ��ɱ���ӣ�-3��ʾ�����ֵ���Χס�� 
int available_judging(int x, int y, int colour)		//x,yΪ���꣬colourΪ�������ӵ���ɫ���루��Ϊ1����Ϊ-1�� 
{
	if (board[x][y] != 0)return 0;			//����õ��������ӣ��������� 
	board[x][y] = colour;			//�Ƚ��õ�����ӱ�� 
	memset(visited_by_air_judging, 0, sizeof(visited_by_air_judging));		//��ʼ�����ʼ�¼����
	if (air_judging(x, y) == 0)		//����õ�û������������
	{
		board[x][y] = 0;		//�ָ��õ�������� 
		return 0;
	}
	for (int i = 0; i < 4; i++)			//����õ����������ĸ����������� 
	{
		int nextx = x + rowmove[i];			//����������������λ�ƺ�õ��������� 
		int nexty = y + colmove[i];
		if (inboard_judging(nextx, nexty) == 1)		//����õ������������������ 
		{
			if (board[nextx][nexty] != 0 && visited_by_air_judging[nextx][nexty] == 0)			//���������ӵ�λ�ã���¼���ʱ�����ѭ���� 
			{
				if (air_judging(nextx, nexty) == 0)		//����ڣ�x,y�����ӻᵼ�������ܵ�����û������������ 
				{
					board[x][y] = 0;		//�ָ��õ�������� 
					return 0;
				}
			}
		}
	}
	board[x][y] = 0;		//�ָ��õ�������� 
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
//�Ե�ǰ�������������������ɫΪcolour��һ�������ӵ�λ����Ŀ 
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
//�жϵ�ǰ�������ɫΪcolour��һ�������ƶ� 

int main()
{
	srand((unsigned)time(0));
	string str;
	int x, y;
	// ����JSON
	getline(cin, str);
	//getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	// �����Լ��յ���������Լ���������������ָ�״̬
	int turnID = input["responses"].size();			//trunIDΪ�Լ����ߵĲ���
	for (int i = 0; i < turnID; i++)
	{
		x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();		//requestsΪ�Է��Ĺ�������
		if (x != -1) board[x][y] = 1;
		x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();			//responsesΪ�����Ĺ������
		if (x != -1) board[x][y] = -1;
	}
	x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
	if (x != -1) board[x][y] = 1;
	// �������JSON
	Json::Value ret;
	Json::Value action;

	//����Ϊ���� 
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