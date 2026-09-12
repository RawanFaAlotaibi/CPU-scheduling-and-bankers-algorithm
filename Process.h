#ifndef PROCESS_H
#define PROCESS_H
#include <vector>
#include <string>
using namespace std;

struct Process {
    int processID, arrivalTime, burstTime, ProcPriority;
    int CompletionTime,WaitingTime,TurnaroundTime;
};

void FCFS(vector<Process> process);
void SJF(vector<Process> process);
void Preemptive(vector<Process> process);
void NonPreemptive(vector<Process> process);
void RR(vector<Process> process, int TimeQuantum);


// دالة الطباعة الموحده بدلاً من الطباعة المتكرره
void printSchedulerResults(const vector<Process>& process, const string& algorithmName);

#endif // PROCESS_H