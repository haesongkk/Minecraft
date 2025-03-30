#include "Process.h"
int main()
{
	Process* pProcess = new Process;
	pProcess->Init();
	pProcess->Update();
	pProcess->Final();
	delete pProcess;
}