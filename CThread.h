#ifndef __CTHREAD_H
#define __CTHREAD_H

#include <queue>
#include <string>
#include <pthread.h>
#include <iostream>
using namespace std;

class CTask
{
protected:
	string m_strTaskName;						//任务名称
	void *m_ptrData;							//任务数据
public:
	CTask(){};
	CTask(string taskName){
		this->m_strTaskName = taskName;
		m_ptrData = NULL;
	}
	virtual int Run()=0;
	void SetData(void *data);					//设置任务数据
};

class CThreadPool
{
private:
	int m_threadNum;							//线程池最大线程数（需初始化赋值）
	static pthread_mutex_t m_threadMutex; 		//线程池互斥锁
	static pthread_mutex_t m_taskMutex;			//任务队列互斥锁
	static pthread_cond_t  m_threadCond;		
	static pthread_cond_t  m_taskCond;
	static queue<CTask *>* taskQueue1;			//任务队列1
	static queue<CTask *>* taskQueue2;			//任务队列2
	static queue<CTask *>* taskback;			//任务返回队列
public:
	CThreadPool(int);							
	static int swapqueue(queue<CTask *>* &a,queue<CTask *>* &b);		//交换两个任务队列
	static void* ThreadFunc(void *arg);									//线程函数
	int Create();														//创建线程池
	int AddTask(CTask*);												//添加任务
	CTask* GetTaskback();												//获得任务运行结果
	int Check();														//检查任务是否全部执行完
	static int clearQueue(queue<CTask*>*);								//清除任务队列
	~CThreadPool();														//拆除线程池
};


#endif


