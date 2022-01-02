#include<iostream>
#include<fstream>
#include<pthread.h>
#include<semaphore.h>
#include<vector>
#include<unistd.h>
using namespace std;
#define P sem_wait(&file_x)
#define V sem_post(&file_x) 
typedef struct order{
   char rw;
   int spendtime;
}order;
typedef struct thread{
   int number;
   double create;
   int last;
}thread;
vector<order> orders;
int orderNum=20;

int shared_data=0;
int read_count;
sem_t rp_wrt;//control buffer
sem_t mutex;//control reader
sem_t file_x;//file lock
void *writer(void *param);
void *reader(void *param);
ofstream outfile("output.txt");

int main()
{
   ifstream file("data.txt");
   order t;
   for(int i=0;i<orderNum;i++)
   {
      file>>t.rw>>t.spendtime;
      orders.push_back(t);
   }
   pthread_t *p = (pthread_t *)malloc(orderNum * sizeof(pthread_t));
   int *p_id = (int *)malloc(orderNum * sizeof(int));
   sem_init(&mutex,0,1);
   sem_init(&rp_wrt,0,1);
   sem_init(&file_x,0,1);
   for(int i = 0; i < orderNum; i++)
   {
      if(orders[i].rw == 'R') //创建读进程
      {
         p_id[i] = i + 1;
         pthread_create(&p[i], NULL, reader, &p_id[i]);
      }
      else//创建写进程
      {
         p_id[i] = i + 1;
         pthread_create(&p[i], NULL, writer, &p_id[i]);
      }
   }
   //等待子进程全部结束后主进程结束
   for(int i = 0; i < orderNum; i++)
   {
      pthread_join(p[i], NULL);
   }

   sem_destroy(&mutex);
   sem_destroy(&rp_wrt);
   sem_destroy(&file_x);
   file.close();
   outfile.close();
   return 0;
}

void *writer(void *param)
{
   thread *t_demo=(thread *)param;
   int id=t_demo->number;
   int last=orders[id-1].spendtime*100000;
   P;
   outfile<<"Write Thread " <<id<<": is created!"<<endl;
   cout<<"Write Thread " <<id<<": is created!"<<endl;
   V;

   sem_wait(&rp_wrt);
   P;
   outfile<<"Write Thread " <<id<<": Enter critical section"<<endl;
   cout<<"Write Thread " <<id<<": Enter critical section"<<endl;
   V;
   usleep(last);
   shared_data=id;
   P;
   outfile<<"Write Thread " <<id<<": writing my id: "<<shared_data<<endl;
   cout<<"Write Thread " <<id<<": writing my id: "<<shared_data<<endl;
   V;
   P;
   outfile<<"Write Thread " <<id<<": Leave critical section"<<endl;
   cout<<"Write Thread " <<id<<": Leave critical section"<<endl;
   V;

   sem_post(&rp_wrt);
}

void *reader(void *param)
{
   thread *t_demo=(thread *)param;
   int id=t_demo->number;
   int last=orders[id-1].spendtime*100000;
   P;
   outfile<<"Read Thread " <<id<<": is created!"<<endl;
   cout<<"Read Thread " <<id<<": is created!"<<endl;
   V;
   sem_wait(&mutex);
   read_count++;
   if(read_count==1)
      sem_wait(&rp_wrt);
   sem_post(&mutex);
   P;
   outfile<<"Read Thread " <<id<<": Enter critical section"<<endl;
   cout<<"Read Thread " <<id<<": Enter critical section"<<endl;
   V;
   usleep(last);
   P;
   outfile<<"Read Thread " <<id<<": read shared data: "<<shared_data<<endl;
   cout<<"Read Thread " <<id<<": read shared data: "<<shared_data<<endl;
   V;
   P;
   outfile<<"Read Thread " <<id<<": Leave critical section"<<endl;
   cout<<"Read Thread " <<id<<": Leave critical section"<<endl;
   V;
   sem_wait(&mutex);

   read_count--;
   if(read_count==0)
      sem_post(&rp_wrt);
   sem_post(&mutex);
}