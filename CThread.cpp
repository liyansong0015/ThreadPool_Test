#include "CThread.h"
#include <string>
#include <iostream>
using namespace std;

/*======================================================================================*/						
pthread_mutex_t CThreadPool::m_threadMutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t CThreadPool::m_taskMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  CThreadPool::m_threadCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  CThreadPool::m_taskCond = PTHREAD_COND_INITIALIZER;
queue<CTask *>* CThreadPool::taskQueue1 = new queue<CTask *>[1];
queue<CTask *>* CThreadPool::taskQueue2 = new queue<CTask *>[1];
queue<CTask *>* CThreadPool::taskback = new queue<CTask *>[1];
/*======================================================================================*/
//设置任务数据
void CTask::SetData(void * data)
{
	m_ptrData = data;	
}


/*======================================================================================*/
//交换两个任务队列
int CThreadPool::swapqueue(queue<CTask *>* &a,queue<CTask *>* &b)
{
	queue<CTask *>* temp = NULL;
	temp = a;
	a = b;
	b= temp;
	cout<<"swao!!!!"<<endl;
}

/*======================================================================================*/
//线程函数
void* CThreadPool::ThreadFunc(void *arg)
{
	pthread_t tid = pthread_self();
	while(1)
	{
		CTask *task;
		if (taskQueue1->empty() && (!taskQueue2->empty())) {
			swapqueue(taskQueue1, taskQueue2);
		}
		pthread_mutex_lock(&m_threadMutex);
		if (taskQueue1->empty()) {
			pthread_cond_wait(&m_threadCond, &m_threadMutex);
			pthread_mutex_unlock(&m_threadMutex);
			continue;
		}

		task = taskQueue1->front();
		taskQueue1->pop();
		pthread_mutex_unlock(&m_threadMutex);

		task->Run();

		pthread_mutex_lock(&m_taskMutex);
		taskback->push(task);
		pthread_mutex_unlock(&m_taskMutex);
		
	}
	return 0;
}

/*======================================================================================*/
//创建线程池
int CThreadPool::Create()
{
	pthread_t tid = 0;
	for(int i=0; i<m_threadNum; ++i) {
		if (pthread_create(&tid, NULL,ThreadFunc,NULL) != 0) {
			cout<<"pthread_create error";
			return -1;
		}
	}
	return 0;
}

/*======================================================================================*/
//添加任务
int CThreadPool::AddTask(CTask* task)
{
	if (pthread_mutex_trylock(&m_threadMutex) == 0) {
		taskQueue1->push(task);
		pthread_cond_signal(&m_threadCond);
		cout<<"add to queue1"<<endl;
		pthread_mutex_unlock(&m_threadMutex);
	}
	else {
		taskQueue2->push(task);
		pthread_cond_signal(&m_threadCond);
		cout<<"add to queue2"<<endl;
		pthread_mutex_unlock(&m_threadMutex);
	}
	return 0;
}

/*======================================================================================*/
//取得任务结果
CTask* CThreadPool::GetTaskback()
{
	CTask *task;

	if (!taskback->empty()) {
		task = taskback->front();
		taskback->pop();
		return task;
	}
	return NULL;
}

/*======================================================================================*/
//防止还有任务未执行到
int CThreadPool::Check()
{
 if(!taskQueue1->empty() || ! taskQueue2->empty())
	pthread_cond_signal(&m_threadCond);
}

/*======================================================================================*/
//删除任务队列
int CThreadPool::clearQueue(queue<CTask*>* a)
{
	while(!a->empty()) {
		a->pop();
	}
	delete a;
	return 0;
}

/*======================================================================================*/
//初始化最大线程数
CThreadPool::CThreadPool(int threadNum)
{
	this->m_threadNum = threadNum;
	Create();
}

/*======================================================================================*/
//拆除线程池
CThreadPool::~CThreadPool()
{
	clearQueue(taskQueue1);
	clearQueue(taskQueue2);
	pthread_cond_destroy(&m_threadCond);
}
