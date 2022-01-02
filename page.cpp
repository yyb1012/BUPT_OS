#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<iomanip>
#define ORDER_NUM 200
using namespace std;
int inum;
int block_num;//内存块数量
int phy_memory[ORDER_NUM];//物理内存
int page_fault;//页面失效次数
double miss;//缺页率
int order[ORDER_NUM];//指令序列
int page[ORDER_NUM];//指令对应页面

void OPT();//Optimal
void LRU();//Least Recently Used
void FIFO();//Fist In First Out

void show_info();
int is_exit(int page);//检测页面是否在内存块中
int find_empty();//寻找空余内存块
void init_ins();//初始化指令序列
void to_page();//指令序列变换为页地址流
void initMemory(int n);//初始化内存块

int main()
{
	init_ins();
	to_page();
	int n;
	while (1)
	{
		printf("请输入内存块数量(1-page_num): ");
		cin >> n;
		initMemory(n);
		printf("\n---------OPT算法：---------\n");
		OPT();
		initMemory(n);
		printf("\n---------LRU算法：---------\n");
		LRU();
		initMemory(n);
		printf("\n---------FIFO算法：---------\n");
		FIFO();
	}
	return 0;
}

void init_ins()//初始化指令序列
{
	srand((unsigned)time(NULL));
	int a = rand() % ORDER_NUM; //0-99
	int rate = rand() % 10;//0~9,模拟概率
	int tmp = 0;
	order[0] = 0;
	for (int i = 1; i < ORDER_NUM; i++)
	{
		if(a==0)
		{
			while(rate==7)
				rate = rand() % 10;
		}
		if (rate <= 6)
		{
			a++;
			order[i] = a;
			if (a == ORDER_NUM-1)//只能向前找
				rate = 7;
			else
				rate = rand() % 10;
		}
		else if (rate == 7)//10%的概率
		{
			tmp = rand() % (a);//向前找
			a = tmp;
			order[i] = a;
			if(a==0)
			{
				while(rate==7)
					rate = rand() % 10;
			}
		}
		else if (rate >= 8)//%20的概率
		{
			tmp = rand() % (ORDER_NUM - a ) + a ; //向后找
			a = tmp;
			order[i] = a;
			if (a == ORDER_NUM-1)
				rate = 7;
			else
				rate = rand() % 10;
		}
	}
	printf("指令序列为:");
	for (int i = 0; i < ORDER_NUM; i++)
	{
		if (i % 20 == 0)
			printf("\n");
		cout << setw(3) << order[i] << " ";
	}
	printf("\n");
}
void to_page()//指令序列变换为页地址流
{
	int page_index;
	printf("请输入每页指令数：（1-ORDER_NUM）\n");
	cin >> inum;
	for (int i = 0; i < ORDER_NUM; i++)
		page[i] = order[i] / inum;
	printf("页面地址流为:");
	for (int i = 0; i < ORDER_NUM; i++)
	{
		page_index = i / inum;
		if ( i % inum == 0)
			cout << "\nPage " << setw(2) << page_index << ": " ;
		cout << setw(3) << page[i] << " ";
	}
	printf("\n");
}
void initMemory(int n)//初始化内存块
{
	block_num = n;
	for (int i = 0; i < block_num; i++)
		phy_memory[i] = -1;
}
int is_exit(int page)//检测页面是否在内存块中
{
	for (int i = 0; i < block_num; i++)
	{
		if (phy_memory[i] == page)
			return i;
	}
	return -1;
}
int find_empty()//寻找空余内存块
{
	for (int i = 0; i < block_num; i++)
	{
		if (phy_memory[i] == -1)
			return i;
	}
	return -1;
}
void show_info()
{
	//printf("内存块：");
	//for (int i = 0; i < block_num; i++) 
	//{
	//	if (phy_memory[i] != -1) 
	//		printf("%d ", phy_memory[i]);
	//}
	//printf("\n");
	;
}
void OPT()
{
	int flag = -1;//内存中是否存在该页面
	int empty;//空闲内存块号
	page_fault = 0;
	int* distance = new int[block_num];
	for (int i = 0; i < block_num; i++)
		distance[i] = 0;
	for (int i = 0; i < ORDER_NUM; i++)
	{
		flag = is_exit(page[i]);
		if (flag != -1)
			//printf("当前请求页号%d已在内存中\n", page[i]);
			;
		else//缺页
		{
			//首先重置distance
			for (int i = 0; i < block_num; i++)
				distance[i] = 0;
			page_fault++;
			empty = find_empty();
			if (empty != -1)
			{//存在空内存块
				phy_memory[empty] = page[i];
				show_info();
			}
			else
			{
				for (int j = 0; j < block_num; j++)
				{
					for(int k = i; k < ORDER_NUM;k++)
					{
						if (phy_memory[j] != page[k])
							distance[j]++;
						else
							break;
					}
				}
				int temp = 0;
				for (int i = 0; i < block_num; i++)
				{
					if (distance[i] >= distance[temp])
						temp = i;
				}
				//printf("当前请求页号%d换出页号%d\n", page[i], phy_memory[temp]);
				phy_memory[temp] = page[i];//调换页面
				show_info();
			}
		}
	}
	miss = page_fault * 1.0 / ORDER_NUM;
	printf("OPT算法缺页次数为：%d, 缺页率为 %.4lf\n", page_fault, miss);
	delete[]distance;
}
void LRU()
{
	int* used = new int[block_num+2];
	for (int i = 0; i < block_num; i++)
		used[i] = 0;
	int flag = -1;//内存中是否存在该页面
	int empty;//空闲内存块号
	int max_index = -1;//最长间隔下标
	int max_used = 0;//最长间隔
	page_fault = 0;
	for (int i = 0; i < ORDER_NUM; i++)
	{
		flag = is_exit(page[i]);
		for (int j = 0; j < block_num; j++)
		{
			if (used[j] >= max_used)
			{
				max_used = used[j];
				max_index = j;
			}
		}
		if (flag != -1)
		{//已存在，则使用次数++
			//printf("当前请求页号%d已在内存中\n", page[i]);
			for (int j = 0; j < block_num; j++)
				used[j]++;
			used[flag] = 0;
		}
		else
		{
			page_fault++;
			empty = find_empty();
			if (empty != -1)//有空内存块
			{
				phy_memory[empty] = page[i];
				show_info();
				used[empty] = 0;
				for (int j = 0; j < empty; j++)
					used[j]++;
			}
			else//无空闲
			{
				//printf("当前请求页号%d请求换出页号%d\n", page[i], phy_memory[max_index]);
				phy_memory[max_index] = page[i];
				used[max_index] = 0;
				for (int j = 0; j < block_num; j++)
					used[j]++;
				max_index = -1;
				max_used = 0;
				show_info();
			}
		}
	}
	miss = page_fault * 1.0 / ORDER_NUM;
	printf("LRU算法缺页次数为：%d, 缺页率为 %.4lf\n", page_fault, miss);
	delete[]used;
}
void FIFO()
{
	int* fre = new int[block_num + 2];
	for (int i = 0; i < block_num; i++)
		fre[i] = 0;
	int flag = -1;//内存中是否存在该页面
	int empty;//空闲内存块号
	int max_index = -1;//最长间隔下标
	int max_used = 0;//最长间隔
	page_fault = 0;
	for (int i = 0; i < ORDER_NUM; i++)
	{
		flag = is_exit(page[i]);
		for (int j = 0; j < block_num; j++)
		{
			if (fre[j] >= max_used)
			{
				max_used = fre[j];
				max_index = j;
			}
		}
		if (flag != -1)
		{//已存在，则使用次数++
			//printf("当前请求页号%d已在内存中\n", page[i]);
			for (int j = 0; j < block_num; j++)
				fre[j]++;
		}
		else
		{
			page_fault++;
			empty = find_empty();
			if (empty != -1)//有空内存块
			{
				phy_memory[empty] = page[i];
				show_info();
				fre[empty] = 0;
				for (int j = 0; j < empty; j++)
					fre[j]++;
			}
			else//无空闲
			{
				//printf("当前请求页号%d请求换出页号%d\n", page[i], phy_memory[max_index]);
				phy_memory[max_index] = page[i];
				fre[max_index] = 0;
				for (int j = 0; j < block_num; j++)
					fre[j]++;
				max_index = -1;
				max_used = 0;
				show_info();
			}
		}
	}
	miss = page_fault * 1.0 / ORDER_NUM;
	printf("FIFO算法缺页次数为：%d, 缺页率为 %.4lf\n", page_fault, miss);
	delete[]fre;
}
