#include<iostream>
#include<fstream>
#include<pthread.h>
#include<semaphore.h>
#include<vector>
#include<unistd.h>
using namespace std;
#define P sem_wait(&file_x)
#define V sem_post(&file_x) 
#define MAX
typedef struct order{
   char rw;
   int spendtime;
}order;
struct thread{
   int number;
   int last;
};
vector<order> orders;
int orderNum=20;

int shared_data=0;
int read_count;
int write_count;
sem_t rp_wrt;
sem_t cs_read;//signal to reader wait in line
sem_t mutex_w;//
sem_t mutex_r;
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
   sem_init(&mutex_w,0,1);
   sem_init(&mutex_r,0,1);
   sem_init(&cs_read,0,1);
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

   sem_destroy(&mutex_w);
   sem_destroy(&mutex_r);
   sem_destroy(&cs_read);
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
   sem_wait(&mutex_w); //互斥访问 write_count
   write_count++;
   if(write_count == 1) //如果是第一个写进程,申请获取读进程排队权限
      sem_wait(&cs_read);
   sem_post(&mutex_w);

   sem_wait(&rp_wrt);
   P;
   outfile<<"Write Thread " <<id<<": Enter critical section"<<endl;
   cout<<"Write Thread " <<id<<": Enter critical section"<<endl;
   V;
   usleep(last);
   //申请缓冲区访问权限
   shared_data=id;
   P;
   outfile<<"Write Thread " <<id<<": writing my id: "<<shared_data<<endl;
   cout<<"Write Thread " <<id<<": writing my id: "<<shared_data<<endl;
   V;
   sem_post(&rp_wrt);
   P;
   outfile<<"Write Thread " <<id<<": Leave critical section"<<endl;
   cout<<"Write Thread " <<id<<": Leave critical section"<<endl;
   V;
   sem_wait(&mutex_w);
   write_count--;
   if(write_count == 0)
      sem_post(&cs_read); //如果是最后一个写进程,释放读进程排队权限,允许其排队访问
   sem_post(&mutex_w);
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
   usleep(last);
   sem_wait(&cs_read);//申请排队权限
   sem_wait(&mutex_r);//互斥访问 read_count
   read_count++;
   P;
   outfile<<"Read Thread " <<id<<": Enter critical section"<<endl;
   cout<<"Read Thread " <<id<<": Enter critical section"<<endl;
   V;
   if(read_count == 1) //如果是第一个读者,申请访问权限
      sem_wait(&rp_wrt);
   sem_post(&mutex_r);
   sem_post(&cs_read);
   P;
   outfile<<"Read Thread " <<id<<": read shared data: "<<shared_data<<endl;
   cout<<"Read Thread " <<id<<": read shared data: "<<shared_data<<endl;
   V;
   P;
   outfile<<"Read Thread " <<id<<": Leave critical section"<<endl;
   cout<<"Read Thread " <<id<<": Leave critical section"<<endl;
   V;
//释放排队权限
/* 访问 shared data,输出读取的信息 */
   sem_wait(&mutex_r);
   read_count--;
   if(read_count == 0)
      sem_post(&rp_wrt);
   sem_post(&mutex_r);
}