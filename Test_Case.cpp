#include "CThread.h"
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
using namespace std;

class CWorkTask:public CTask{
public:
	CWorkTask(){}
	int Run() {
		cout<<(char*)this->m_ptrData<<endl;
		sleep(1);
		return 0;
	}
};


int main()
{
	struct timeval begin,end;
	CWorkTask tt;
	char str[]="!!!!!!!this is test";

	tt.SetData((void*)str);
	CThreadPool threadpool(5);

	for (int i=0; i<10; ++i) {
		threadpool.AddTask(&tt);
	}
	while(1)
	{
		sleep(1);
		threadpool.Check();
	}
	return 0;
}
