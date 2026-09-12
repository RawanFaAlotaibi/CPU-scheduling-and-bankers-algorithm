#include "Process.h"
#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>
#include <limits>
#include <climits>
#include <algorithm>
#include <tuple> // لتخزين تسلسل مخطط جانت
using GanttSegment = tuple<int, int, int>;
using namespace std;


void print_gantt_chart(const vector<GanttSegment>& seq) {
    if (seq.empty()) return;
    
    cout << "\n--- Gantt Chart ---\n";
   // العمليات
    cout << "|";
    for (const auto& segment : seq) {
        cout << "--P" << get<0>(segment) << "--|"; 
    }
    cout << "\n";
//الاوقات 
    cout << std::get<1>(seq[0]) << " "; 
    
    // حلقة لطباعة وقت الانتهاء 
    for (const auto& segment : seq) {
        cout << get<2>(segment) << " ";
    }
    cout << endl;
}
//********************************************
void FCFS(vector<Process> process) {
    int n = process.size();
    vector<int> wait(n), turn(n), start(n), comp(n);
    // الفرز حسب وقت الوصول
    sort(process.begin(), process.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    start[0] = process[0].arrivalTime; // زمن بدء العملية هو نفس وقت وصولها 
    comp[0] = start[0] + process[0].burstTime; //زمن الاكمال = زمن البدء بالاضافة الى وقت التنفيذ المطلوب
    vector<GanttSegment> seq;
    wait[0] = 0; // العملية الاولى ما انتظرت شي 
    turn[0] = comp[0] - process[0].arrivalTime; //زمن الدوران = زمن الاكمال مطروح منه وقت وصولها 
    seq.push_back({process[0].processID, start[0], comp[0]});
    
    for (int i = 1; i < n; i++) { //تبدا من ثاني عملية لاخر عملية 
        start[i] = max(process[i].arrivalTime, comp[i-1]); // تاخذ الماكس بين وقت وصولها وانتهاء العملية السابقة 
        comp[i] = start[i] + process[i].burstTime; //زمن الاكمال للعملية الحالية هو زمن البدء بالاضافة الى وقت التنفيذ
        wait[i] = start[i] - process[i].arrivalTime;
        turn[i] = comp[i] - process[i].arrivalTime;
        seq.push_back({process[i].processID, start[i], comp[i]});
    }
    // مرحلة حفظ النتائج
    for (int i = 0; i < n; i++) {
        process[i].CompletionTime = comp[i];
        process[i].WaitingTime = wait[i];
        process[i].TurnaroundTime = turn[i];
    }
    // طباعة النتائج
    printSchedulerResults(process, "First-Come, First-Served");
    print_gantt_chart(seq);
}
//***************************************************************
void SJF(vector<Process> process) {
    int n = process.size();
    vector<int> rem(n), wait(n), turn(n), comp(n);
    vector<bool> done(n, false);
    vector<GanttSegment> seq;
    for (int i = 0; i < n; i++) rem[i] = process[i].burstTime; // تهيئة الوقت المتبقي بوقت التنفيذ الكلي 
    int t = 0, finished = 0; // t: عدد العمليات التي اكتملت ,الوقت الحالي 
    
    while (finished < n) { // حلقة الجدولة الرئيسية تستمر حتى تنتهي جميع العمليات 
        int idx = -1, best = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (process[i].arrivalTime <= t && !done[i] && rem[i] < best) {
                best = rem[i];
                idx = i;
            }
        }
        if (idx == -1) {
            t++;
            continue;  }
        // الخوارزمية غير قابلة للمقاطعة لذلك تبدا العملية وتستمر حتى تنتهي 
        int start = t;
        t += rem[idx];
        comp[idx] = t;
        seq.push_back({process[idx].processID, start, comp[idx]});
//حساب النتائج النهائية للعملية المكتملة 
        wait[idx] = start - process[idx].arrivalTime;
        turn[idx] = comp[idx] - process[idx].arrivalTime;
       
        done[idx] = true;
        finished++;
    }
    
    for (int i = 0; i < n; i++) {
        process[i].CompletionTime = comp[i];
        process[i].WaitingTime = wait[i];
        process[i].TurnaroundTime = turn[i];
    }
    
    printSchedulerResults(process, "Shortest Job First (Non-Preemptive)");
    print_gantt_chart(seq);
}
//****************************************************************************
void Preemptive(vector<Process> process) {
    int n = process.size();
    vector<int> rem(n), wait(n), turn(n), comp(n);
    vector<bool> done(n, false);
    for (int i = 0; i < n; i++) rem[i] = process[i].burstTime;
    
    int t = 0, finished = 0;
    vector<GanttSegment> seq;
    int last_idx = -1;
    while (finished < n) {
        int idx = -1, best = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (process[i].arrivalTime <= t && !done[i] && process[i].ProcPriority < best) {
                best = process[i].ProcPriority;
                idx = i;
            }
        }
        if (idx == -1) {
            t++;
            continue;
        }
        if (idx != last_idx) { 
            if (!seq.empty()) { 
                get<2>(seq.back()) = t; 
            }  
                seq.push_back({process[idx].processID, t, 0}); 
                 last_idx = idx; 
        }
        
        rem[idx]--;
        t++;
        
        if (rem[idx] == 0) {
            
           done[idx] = true;
            comp[idx] = t;
            turn[idx] = t - process[idx].arrivalTime;
            wait[idx] = turn[idx] - process[idx].burstTime;
            finished++;
        
        if (!seq.empty() && get<0>(seq.back()) == process[idx].processID) {
                get<2>(seq.back()) = t; 
            }
            last_idx = -1;
         }
    }
    for (int i = 0; i < n; i++) {
        process[i].CompletionTime = comp[i];
        process[i].WaitingTime = wait[i];
        process[i].TurnaroundTime = turn[i];
    }
    
    printSchedulerResults(process, "Priority (Preemptive)");
    if (!seq.empty() && get<2>(seq.back()) == 0) { 
        get<2>(seq.back()) = t;
    }
     print_gantt_chart(seq);
    
}

// ********************************************************
void NonPreemptive(vector<Process> process) {
    int n = process.size();
    vector<int> wait(n), turn(n), comp(n);
    vector<bool> done(n, false);
    int t = 0, completed = 0;
    vector<GanttSegment> seq;
    while (completed < n) {
        int idx = -1, min_p = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (process[i].arrivalTime <= t && !done[i] && process[i].ProcPriority < min_p) {
                min_p = process[i].ProcPriority;
                idx = i;
            }
        }
        if (idx == -1) {
            t++;
            continue;
        }
        
        int start = t;
        t += process[idx].burstTime;
        comp[idx] = t;
        seq.push_back({process[idx].processID, start, comp[idx]});
        wait[idx] = start - process[idx].arrivalTime;
        turn[idx] = comp[idx] - process[idx].arrivalTime;
        done[idx] = true;
        completed++;
    }
    
    for (int i = 0; i < n; i++) {
        process[i].CompletionTime = comp[i];
        process[i].WaitingTime = wait[i];
        process[i].TurnaroundTime = turn[i];
    }
    
    printSchedulerResults(process, "Priority (Non-Preemptive)");
    print_gantt_chart(seq);
}
// *********************************************************************
void RR(vector<Process> process, int TimeQuantum) {
    int n = process.size();
    vector<int> rem(n), wait(n), turn(n), comp(n);
    vector<bool> done(n, false), seen(n, false);
    int finished = 0;
    
    for (int i = 0; i < n; i++) {
     rem[i] = process[i].burstTime;
    }
   
    queue<int> q;
    int t = 0; 
   
       for (int i = 0; i < n; i++) {
         if (process[i].arrivalTime ==0) {
            q.push(i);
            seen[i] = true;
         }
        }
    
        vector<GanttSegment> seq;
    while (finished < n ){
       
       if(q.empty()) {
        int next_arrival = INT_MAX;
         for (int i = 0; i < n; i++) {
            if (!seen[i] && !done[i]) {
                next_arrival = min(next_arrival, process[i].arrivalTime);
             }
            }
        
        if (next_arrival == INT_MAX) {
            break; 
        }
        t = max(t, next_arrival);
         for (int i = 0; i < n; i++) {
            if (process[i].arrivalTime <= t && !seen[i] && !done[i])  {
                q.push(i);
                seen[i] = true;
                }
            }
        
        if (q.empty()) {
            continue;
         }
        }
        
        int idx = q.front();
        q.pop();
        int start_time = t;
        int exec_time = min(rem[idx], TimeQuantum);
        t += exec_time;
        rem[idx] -= exec_time;
        seq.push_back({process[idx].processID, start_time, t});
        
        for (int i = 0; i < n; i++) {
            if (process[i].arrivalTime <= t && !seen[i] && !done[i]) { 
            q.push(i);
            seen[i] = true;
            }
        }
         if (rem[idx] == 0){
            done[idx] = true;
            comp[idx] = t;
            turn[idx] = t - process[idx].arrivalTime;
            wait[idx] = turn[idx] - process[idx].burstTime;
            finished++;
        }else {
        q.push(idx);
        }
     }    
         for (int i = 0; i < n; i++) {
        process[i].CompletionTime = comp[i];
        process[i].WaitingTime = wait[i];
        process[i].TurnaroundTime = turn[i];
        }
        printSchedulerResults(process, "Round Robin");
        print_gantt_chart(seq);
   
}    
// دالة الطباعة الموحده
//******************************************************
void printSchedulerResults(const vector<Process>& process, const string& algorithmName) {
    if (process.empty()) return;
    int n = process.size();
    double total_w = 0;
    double total_t = 0;

    cout << "\n--- " << algorithmName << " Scheduling ---\n";
    cout << "Process | AT\t| BT\t| Priority| CT\t| TAT\t| WT\n";
    cout << "------------------------------------------------------------------\n";
    for (const auto& p : process) {
        total_w += p.WaitingTime;
        total_t += p.TurnaroundTime;
        cout << "P" << p.processID << "\t| " << p.arrivalTime << "\t| " 
                  << p.burstTime << "\t| " << p.ProcPriority << "\t  | " 
                  << p.CompletionTime << "\t| " << p.TurnaroundTime << "\t| " 
                  << p.WaitingTime << "\n";
    }
    cout << "------------------------------------------------------------------\n";
    // حساب وطباعة المتوسطات مرة واحدة
    cout << fixed << setprecision(2);
    cout << "Average Waiting Time: " << total_w / n << "\n";
    cout << "Average Turnaround Time: " << total_t / n << "\n";

}
