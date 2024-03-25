#include<iostream>
#include <fstream>
#include<string>
#include<vector>
#include <queue>
#include <unordered_map>
#include <climits>
using namespace std;

class Process {
    public:
        int id;
        int arrivalTime;
        int cpuBurstTime = 0;   //cpu
        int waitingTime = 0;
        int completionTime = 0;
        int turnaroundTime;
        int processTime = 0;  //cpu + io
        float penaltyRatio;
        vector<int> cpuBurst;
        vector<int> IOBurst;
};

class Compare {
    public:
        bool operator()(Process p1, Process p2){
            if(p1.cpuBurstTime > p2.cpuBurstTime) return true;
            else if(p1.cpuBurstTime == p2.cpuBurstTime && p1.arrivalTime > p2.arrivalTime) return true;
            else return false;
        }
};

int main(int argc, char* argv[]){
    if(argc < 2) cout << "Enter correct arguments." << endl, exit(1);
    vector<Process> processes;
    fstream file;
    file.open(argv[1], ios::in);
    string line;
    int id=1;
    while(file){
        getline(file, line);
        if(line[0] != '<' && line != ""){
            Process p;
            p.id = id++;
            int i=0, indx = 0;
            while(line[i] != '-'){ // - 1
                string num = "";
                while(line[i] != ' ') num += line[i++];
                if(indx == 0) p.arrivalTime = stoi(num);
                else if(indx % 2 == 1) p.cpuBurstTime += stoi(num), p.cpuBurst.push_back(stoi(num)), p.processTime += stoi(num);
                else if(indx % 2 == 0) p.IOBurst.push_back(stoi(num)), p.processTime += stoi(num);
                indx++; 
                while(line[i] == ' ') i++;
            }
            processes.push_back(p);
        }
    }

    int clock = INT_MAX;
    unordered_map<int, vector<Process>> mp;
    for(auto it: processes){
        if(clock > it.arrivalTime) clock = it.arrivalTime;
        mp[it.arrivalTime].push_back(it);
    }

    priority_queue<Process, vector<Process>, Compare> pq_cpu;
    queue<pair<Process, int>> q_io;
    for(auto it: mp[clock]){
        pq_cpu.push(it);
    }
    while(!pq_cpu.empty() || !q_io.empty()){
        int cpuTime = 1;
        if(!pq_cpu.empty()){
            Process running = pq_cpu.top();
            pq_cpu.pop();
            cpuTime = running.cpuBurst[0];
            // cout << running.id << " id" << endl;
            // cout << cpuTime << " " << clock+cpuTime << endl;
            running.cpuBurst.erase(running.cpuBurst.begin());
            running.cpuBurstTime -= cpuTime;
            for(int i=clock+1; i<=clock+cpuTime; i++){
                if(!mp[i].empty()){
                    for(auto it: mp[i]) pq_cpu.push(it);
                }
            }

            //Completion
            if(running.IOBurst.size() == 0 && running.cpuBurst.size() == 0) {
                for(auto &it: processes){
                    if(it.id == running.id) it.completionTime = clock + cpuTime;
                }
            }

            if(!running.IOBurst.empty()) q_io.push({running, clock+cpuTime});
        }

        //IO
        for(int i=clock+1; i<=clock+cpuTime; i++){
            if(!q_io.empty()){
                Process& IoFront = q_io.front().first;
                if(q_io.front().second < i){
                    IoFront.IOBurst.front()--;
                    if(IoFront.IOBurst.front() <= 0) {
                        IoFront.IOBurst.erase(IoFront.IOBurst.begin());
                        //Remove I/O waiting time
                        for(auto &it: processes){
                            if(it.id == IoFront.id) {
                                it.waitingTime -= i-it.IOBurst.front()-q_io.front().second;
                                it.IOBurst.erase(it.IOBurst.begin());
                                break;
                            }
                        }
                        if(!IoFront.cpuBurst.empty()) pq_cpu.push(IoFront);
                        //Completion
                        if(IoFront.IOBurst.empty() && IoFront.cpuBurst.empty()) {
                            for(auto &it: processes){
                                if(it.id == IoFront.id) it.completionTime = i;
                            }
                        }
                        q_io.pop();
                    }
                }
            }
        }
        clock += cpuTime;
    }

    int totalExecutionTime = clock;
    int totalTurnaroundTime = 0;
    int totalWaitingTime = 0;
    float totalPenaltyRatio = 0;
    ofstream output("Results/SJF_output.txt");
    for(auto it: processes){
        output << "Process P" << it.id << ":" << endl;
        it.turnaroundTime = it.completionTime - it.arrivalTime;
        it.waitingTime += it.turnaroundTime-it.processTime; //
        it.penaltyRatio = (float) it.turnaroundTime/it.processTime;
        output << "Turnaround Time: " << it.turnaroundTime << endl;
        output << "Waiting Time: " << it.waitingTime << endl;
        output << "Penalty Ratio: " << it.penaltyRatio << endl;
        output << "Completion Time: " << it.completionTime << endl;
        output << "##################################################\n" << endl;

        totalTurnaroundTime += it.turnaroundTime;
        totalWaitingTime += it.waitingTime;
        totalPenaltyRatio += it.penaltyRatio;
    }

    int totalProcesses = processes.size();
    output << "Average Turnaround Time: " << (float)totalTurnaroundTime/totalProcesses << endl;
    output << "Average Waiting Time: " << (float)totalWaitingTime/totalProcesses << endl;
    output << "Average Penalty Ratio: " << (float)totalPenaltyRatio/totalProcesses << endl;
    output << "Throughput: " << (float) totalProcesses/totalExecutionTime << endl;
    output.close();
    file.close();
}