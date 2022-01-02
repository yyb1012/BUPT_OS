#include<iostream>
#include<fstream>
#include<time.h>
using namespace std;
int main()
{
	int dataNum=20;
	int timeMax=5;
	char RW[2]={'R','W'};
	ofstream data("data.txt");
	if(data)
	{
		srand((unsigned)time(NULL));
		for(int i=0;i<dataNum;i++)
		{
			int index=rand()%2;
			int spendTime=(rand()%timeMax+1);
			data<<RW[index]<<" "<<spendTime;
			if(i<dataNum-1)
				data<<"\n";
		}
	}
	data.close();
}
