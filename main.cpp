#include "Process.h"
#include <iostream>
#include <string>
#include <vector> // لاستخدام المصفوفات لتخزين بيانات العمليات.
using namespace std;

int main() {
    cout << "=== Part 1: CPU Scheduling Project ===\n\n";
    
    int num , TimeQuantum;
    cout << "Please Enter number of processes: ";
    cin >> num;
    
    vector<Process> process(num);
    
    cout << "\n Please Enter process details:\n";
    for (int i = 0; i < num ; i++) {
       process[i].processID = i + 1;
       cout << "\nProcess " << process[i].processID << ":\n";
        cout << "  Arrival Time: ";
        cin >> process[i].arrivalTime;
        cout << "  Burst Time: ";
        cin >> process[i].burstTime;
        cout << "  Priority (lower number = higher priority): ";
        cin >> process[i].ProcPriority;
    }
    
    cout << "\n\n Please Enter time quantum for Round Robin(RR) : ";
    cin >> TimeQuantum;
    //  استدعاء جميع الدوال
    FCFS(process);
    SJF(process);
    Preemptive(process);
    NonPreemptive(process);
    RR(process, TimeQuantum);
 //************************************************************

    cout << "\n\n=== Part 2: Banker's Algorithm ===\n";
    int Resources; 
    cout << "Please Enter the number of resource types: ";
    cin >> Resources;
    
    vector<vector<int>> Max_Matrix(num, vector<int>(Resources));      
    vector<vector<int>> Allocation_Matrix(num, vector<int>(Resources)); 
    vector<int> Available_Vector(Resources);                              

    //  إدخال المدخلات عن طريق المستخدم 
    cout << "\nEnter Maximum Demand Matrix (" << num << "x" << Resources << "):\n";
    for (int i = 0; i < num; i++) {
        cout << "  P" << i << " Max Need: ";
        for (int j = 0; j < Resources; j++) {
            cin >> Max_Matrix[i][j];
        }
    }

    cout << "\nEnter Current Allocation Matrix (" << num << "x" << Resources << "):\n";
    for (int i = 0; i < num; i++) {
        cout << "  P" << i << " Assigned: ";
        for (int j = 0; j < Resources; j++) {
            cin >> Allocation_Matrix[i][j];
        }
    }

    cout << "\nEnter Available Resource Vector: ";
    for (int j = 0; j < Resources; j++) {
        cin >> Available_Vector[j];
    }
    
    // حساب مصفوفة الاحتياج المتبقي (Need)
    vector<vector<int>> Need_Matrix(num, vector<int>(Resources));
    for (int i = 0; i < num; i++)
        for (int j = 0; j < Resources; j++)
            Need_Matrix[i][j] = Max_Matrix[i][j] - Allocation_Matrix[i][j];

    // Safety Algorithm
    // **********************************************
    
    vector<int> is_done(num, 0); 
    vector<int> safe_order;              
    
    // حلقة البحث الرئيسية
    while (true) {
        bool progress_made = false; 
        
        if (safe_order.size() == num) break;
        for (int i = 0; i < num; i++) {
            if (!is_done[i]) { 
                bool is_affordable = true; 
                
                // 1. التحقق من الشرط: Need <= Work (Available_Vector)
                for (int j = 0; j < Resources; j++) {
                    if (Need_Matrix[i][j] > Available_Vector[j]) {
                        is_affordable = false;
                        break;
                    }
                }
                
                if (is_affordable) {
                    // 2. تحرير الموارد: Work = Work + Allocation
                    for (int j = 0; j < Resources; j++)
                        Available_Vector[j] += Allocation_Matrix[i][j];
                    safe_order.push_back(i);
                    is_done[i] = 1;
                    progress_made = true;
                    
                    break; 
                }
            }
        }
        
        //  شرط الخروج في حالة عدم الأمان
        if (!progress_made && safe_order.size() < num) {
            cout << "\nSystem is in an **UNSAFE STATE**." << endl;
            return 0; // إنهاء البرنامج
        }
    }

    // عرض النتائج
    cout << "\nSystem is in a **SAFE STATE**." << endl;
    cout << "Safe Execution Order: ";
    for (size_t i = 0; i < safe_order.size(); i++) {
        cout << "P" << safe_order[i];
        if (i != safe_order.size() - 1) cout << " -> ";
    }
    cout << endl;
    
    return 0;
}